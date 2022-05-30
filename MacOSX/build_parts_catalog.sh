#! /bin/bash

# Functions
function init_vars {
	if [ "${LDRAW_CAT_HI_RES}" != "" ]; then
		export CATALOG_DIR=${HOME}/tmp/LDrawCatalog-high
	else
		export CATALOG_DIR=${HOME}/tmp/LDrawCatalog
	fi
	export LAST_DIR=${CATALOG_DIR}/last
	export IMG_DIR=${CATALOG_DIR}/img
	export FULL_DIR=${CATALOG_DIR}/full
	export CAT_DIR=${CATALOG_DIR}/cat
	export CSS_FILENAME=LDrawPartsCatalog.css
	export MAIN_FILENAME=LDrawPartsCatalog.html
	export FULL_FILENAME=LDrawOfflinePartsCatalog.html
	export CATALOG=${CATALOG_DIR}/${MAIN_FILENAME}
	export FULL_CATALOG=${FULL_DIR}/${FULL_FILENAME}
	export CATEGORY_BASE=${CAT_DIR}/LDraw
	export ALL_PARTS_LIST=${CATALOG_DIR}/AllLDrawParts.txt
	export LAST_ALL_PARTS_LIST=${LAST_DIR}/AllLDrawParts.txt
	export PARTS_LIST=${CATALOG_DIR}/LDrawParts.txt
	export LAST_PARTS_LIST=${LAST_DIR}/LDrawParts.txt
	export BASEPLATES_LIST=${CATALOG_DIR}/LDrawBaseplates.txt
	export LAST_BASEPLATES_LIST=${LAST_DIR}/LDrawBaseplates.txt
	export PANELS_LIST=${CATALOG_DIR}/LDrawPanels.txt
	export LAST_PANELS_LIST=${LAST_DIR}/LDrawPanels.txt
	export CATEGORIES_LIST=${CATALOG_DIR}/Categories.txt
	export LDRAW_DIR=${HOME}/Library/ldraw
	export LDVIEW=/Applications/LDView.app/Contents/MacOS/LDView
}

function print_part_row {
	png_filename=${part_filename%.dat}.png
	png_size=`file "${IMG_DIR}/${png_filename}" | cut -d , -f 2`
	png_width=`echo ${png_size} | cut -d ' ' -f 1`
	png_height=`echo ${png_size} | cut -d ' ' -f 3`
	if [ "${LDRAW_CAT_HI_RES}" != "" ]; then
		(( png_width /= 2 ))
		(( png_height /= 2 ))
	fi
	if [ "${keywords}" != "" ]; then
		rowspan=3
	else
		rowspan=2
	fi
	echo '		<tr valign="top">
			<td rowspan='${rowspan}' align="right"><img width="'${png_width}'" height="'${png_height}'" src="../img/'${png_filename}'" alt="Part Image"/></td>
			<td class="label" align="right" height="99%"><font size="-2">File</font></td><td>'${part_filename}'</td>
		</tr>
		<tr valign="top"><td class="label" align="right"><font size="-2">Name</font></td><td>'${desc}'</td></tr>'
	if [ "${keywords}" != "" ]; then
		echo '		<tr valign="top"><td class="label" align="right"><font size="-2">Keywords</font></td><td>'${keywords}'</td></tr>'
	fi
}

function get_part_desc {
	if [ -e "${1}" ]; then
		part_desc=`head -n 1 ${1} | sed 's/$//' | sed -E 's/^0[ 	]+//' | tr -d '\n'`
		if [[ "${part_desc}" =~ ^~Moved\ to\  ]]; then
			old_part_desc=${part_desc}
			new_part=`echo "${part_desc}" | sed -E 's/~Moved to //'`
			get_part_desc "${new_part}.dat"
		fi
	fi
}

function process_part {
	get_vars_from_line ${*}
	export CATEGORY_FILE=${CATEGORY_BASE}${cat}.html
	print_part_row >> "${CATEGORY_FILE}"
	print_part_row >> "${FULL_CATALOG}"
}

function pre_process_part {
	get_vars_from_line ${*}
	get_part_desc ${part_filename}
	if [[ "${part_desc}" =~ [Bb][Aa][Ss][Ee][Pp][Ll][Aa][Tt][Ee] ]]; then
		if [ "${cmdline}" == "" ]; then
			echo ${part_filename} >> ${BASEPLATES_LIST}
		else
			echo ${part_filename}${cmdline} >> ${BASEPLATES_LIST}-cmdline
		fi
	elif [[ "${part_filename}" =~ ^(4864|6268|4215|2362|4865|4345ap|4345bp)[^0-9] ]]; then
		if [ "${cmdline}" == "" ]; then
			echo ${part_filename} >> ${PANELS_LIST}
		else
			echo ${part_filename}${cmdline} >> ${PANELS_LIST}-cmdline
		fi
	else
		if [ "${cmdline}" == "" ]; then
			echo ${part_filename} >> ${PARTS_LIST}
		else
			echo ${part_filename}${cmdline} >> ${PARTS_LIST}-cmdline
		fi
	fi
}

function find_meta_value {
	meta_value=`grep -E '^[ 	]*0[ 	]+\'$2'[ 	]+' $1 | sed -E -e 's/^[ 	]*0[ 	]+\'$2'[ 	]+//' -e 's///g'`
}

function find_category {
	find_meta_value $1 "!CATEGORY"
	if [ "${meta_value}" == "" ]; then
		part_cat=`echo ${orig_part_desc} | cut -f 1 -d ' ' | sed -E 's/^[_~=]+//'`
	else
		part_cat=$meta_value
	fi
}

function find_cmdline {
#	find_meta_value $1 "!CMDLINE"
#	if [ "${meta_value}" != "" ]; then
#		part_cmdline=${meta_value}
#	else
#		unset part_cmdline
#	fi
	unset part_cmdline
}

function find_keywords {
	meta_value=`grep -E '^[ 	]*0[ 	]+\!KEYWORDS[ 	]+' $1 | sed -E -e 's/^[ 	]*0[ 	]+\!KEYWORDS[ 	]+//' -e 's///g' -e 's/$/, /'`
	if [ "${meta_value}" != "" ]; then
		part_keywords=`echo ${meta_value} | sed 's/,$//'`
	else
		unset part_keywords
	fi
}

function print_part_scan {
	orig_part_desc=`head -n 1 ${1} | sed 's/$//' | sed -E 's/^0[ 	]+//' | tr -d '\n'`
	find_category $1
	find_cmdline $1
	find_keywords $1
	echo -e ${part_cat}${orig_part_desc}${1}${part_cmdline}${part_keywords}
}

function scan_part {
	print_part_scan $1 >> ${ALL_PARTS_LIST}
}

function print_style_sheet {
echo 'body
{
	font-family: tahoma, sans-serif;
}

p
{
	margin-top: 0px;
	margin-bottom: 0px;
}

div
{
	position: absolute;
}

table
{
	border-collapse: collapse;
	border: 2px solid #000000;
	background-color: #FFFFFF;
	padding: 0px;
}

tr
{
	page-break-inside: avoid;
}

th
{
	background-color: #0080D0;
	color: #FFFFFF;
	border-bottom: 1px solid #000000;
	border-right: 1px solid #00558A;
	padding: 4px 8px;
}

th.title
{
	background-color: #EEEEEE;
	color: #000000;
	font-size: 150%;
}

td
{
	background-color: #FFFFFF;
	border-bottom: 1px solid #BFBFBF;
	border-right: 1px solid #BFBFBF;
	padding: 2px 4px;
}

td.label
{
	text-align: right;
	border-right: 0px;
	width: 1%;
	padding: 2px 0px 2px 4px;
}

table.credits
{
	border-collapse: collapse;
	border-style: none;
	background-color: transparent;
	margin: 0px;
	width: 100%;
}

td.credits
{
	background-color: #EEEEEE;
	border-style: none;
	padding: 0px;
}

table.credits td
{
	background-color: transparent;
	color: #808080;
	border-style: none;
	font-size: 65%;
}

a
{
	text-decoration: none;
	border-style: none;
}

a:link
{
	color: #000080;
}

a:visited
{
	color: #0000FF;
}

a:hover
{
	text-decoration: underline;
	color: #000080;
}

img
{
	float: right;
}

a img
{
	border-style: none;
}

:link:hover img
{
	background-color: #D0E8FF;
}

:visited:hover img
{
	background-color: #A0C0FF;
}'
}

function create_style_sheet {
	print_style_sheet > "${CATALOG_DIR}/${CSS_FILENAME}"
}

function print_header {
echo '<!DOCTYPE html>
<html>
<head>
<title>'${1}'</title>'
echo '<style type="text/css" title="Standard"><!--'
print_style_sheet
echo '--></style>'
#echo '<link href="'${2}${CSS_FILENAME}'" title="Standard" rel="stylesheet" type="text/css">'
echo '</head>
<body>'
}

function print_cat_header {
	print_header "${*}" ../
	#echo '<p><a href="../'${MAIN_FILENAME}'">Back</a></p>'
	echo '<table>
	<thead>
		<tr>
			<th colspan="3">'${*}'</th>
		</tr>
	</thead>
	<tbody>'
}

function print_full_header {
	print_header "${*}" ../
	echo '<table>
	<tbody>'
}

function print_main_header {
	print_header "${*}"
echo '<table>
	<thead>
		<tr>
			<th>Category</th>
		</tr>
	</thead>
	<tbody>'
}

function create_main_header {
	rm -f ${CATALOG}
	print_main_header LDraw Parts > "${CATALOG}"
}

function create_full_header {
	rm -f ${CATALOG}
	print_full_header LDraw Parts > "${FULL_CATALOG}"
}

function create_cat_header {
	export CATEGORY_FILE=${CATEGORY_BASE}${1}.html
	rm -f ${CATEGORY_FILE}
	print_cat_header ${1} > "${CATEGORY_FILE}"
}

function print_footer {
echo '	</tbody>
</table>
</body>
</html>'
}

function create_cat_footer {
	export CATEGORY_FILE=${CATEGORY_BASE}${1}.html
	print_footer >> "${CATEGORY_FILE}"
}

function create_full_footer {
	print_footer >> "${FULL_CATALOG}"
}

function create_main_footer {
	print_footer >> "${CATALOG}"
}

function create_images {
	image_size=512
	edge_thickness=1
	if [ "${LDRAW_CAT_HI_RES}" != "" ]; then
		image_size=1024
		edge_thickness=2
	fi
	while [ `stat -f %z ${1}` -gt 0 ]
	do
		ldview_command_line="${LDVIEW} -LDrawDir=${LDRAW_DIR} ${2} -FOV=0.1 -SaveSnapshots=1 -SaveDir=${IMG_DIR} -SaveWidth=${image_size} -SaveHeight=${image_size} -EdgeThickness=${edge_thickness} -SaveZoomToFit=1 -AutoCrop=1 -SaveAlpha=1 -LineSmoothing=1 -BFC=0 -PreferenceSet=LDPartsCatalog `head -n 50 ${1}`"
		$ldview_command_line
		if [ $? != 0 ]; then
			# LDView occasionally just stops in the middle. Since that is going
			# to be a really hard bug to track down, retry the LDView command
			# if it fails the first time.
			echo LDView failed. Trying one more time.
			$ldview_command_line
		fi
		tail -n +51 ${1} > ${1}.tmp
		mv ${1}.tmp ${1}
	done
	rm -f "${1}"
	while read line
	do
		get_cmdline_vars_from_line ${line}
		if [[ "${cmdline}" =~ ^-[cC](.*) ]]; then
			default_color=-DefaultColorNumber=${BASH_REMATCH[1]}
		else
			unset default_color
		fi
		ldview_command_line="${LDVIEW} -LDrawDir=${LDRAW_DIR} ${2} -FOV=0.1 -SaveSnapshots=1 -SaveDir=${IMG_DIR} -SaveWidth=${image_size} -SaveHeight=${image_size} -EdgeThickness=${edge_thickness} -SaveZoomToFit=1 -AutoCrop=1 -SaveAlpha=1 -LineSmoothing=1 -BFC=0 ${default_color} ${part_filename}"
		$ldview_command_line
	done < "${1}-cmdline"
	rm -f "${1}-cmdline"
}

function create_empty_list {
	rm -f ${1}
	touch ${1}
}

function create_empty_lists {
	create_empty_list ${ALL_PARTS_LIST}
	create_empty_list ${PARTS_LIST}
	create_empty_list ${PARTS_LIST}-cmdline
	create_empty_list ${BASEPLATES_LIST}
	create_empty_list ${BASEPLATES_LIST}-cmdline
	create_empty_list ${PANELS_LIST}
	create_empty_list ${PANELS_LIST}-cmdline
}

function process_parts {
	while read line
	do
		process_part ${line}
	done < "${ALL_PARTS_LIST}"
}

function pre_process_parts {
	if [ -e ${LAST_PARTS_LIST} ] && [ -e ${LAST_PARTS_LIST}-cmdline ] && [ -e ${LAST_BASEPLATES_LIST} ] && [ -e ${LAST_BASEPLATES_LIST}-cmdline ] && [ -e ${LAST_PANELS_LIST} ] && [ -e ${LAST_PANELS_LIST}-cmdline ]; then
		cp ${LAST_PARTS_LIST} ${PARTS_LIST}
		cp ${LAST_PARTS_LIST}-cmdline ${PARTS_LIST}-cmdline
		cp ${LAST_BASEPLATES_LIST} ${BASEPLATES_LIST}
		cp ${LAST_BASEPLATES_LIST}-cmdline ${BASEPLATES_LIST}-cmdline
		cp ${LAST_PANELS_LIST} ${PANELS_LIST}
		cp ${LAST_PANELS_LIST}-cmdline ${PANELS_LIST}-cmdline
		return
	fi
	echo Preprocessing part information...
	while read line
	do
		pre_process_part ${line}
	done < "${ALL_PARTS_LIST}"
	cp ${PARTS_LIST} ${LAST_DIR}/
	cp ${PARTS_LIST}-cmdline ${LAST_DIR}/
	cp ${BASEPLATES_LIST} ${LAST_DIR}/
	cp ${BASEPLATES_LIST}-cmdline ${LAST_DIR}/
	cp ${PANELS_LIST} ${LAST_DIR}/
	cp ${PANELS_LIST}-cmdline ${LAST_DIR}/
}

function process_command_line {
	if [ "${1}" == "-hi" ]; then
		echo Generating high-resolution images.
		LDRAW_CAT_HI_RES=1
	else
		unset LDRAW_CAT_HI_RES
	fi
}

function verify_dir {
	if [ ! -d "${1}" ]; then
		mkdir -p "${1}"
	fi
	if [ ! -d "${1}" ]; then
		echo Error creating directory ${1}.
		exit
	fi
}

function verify_settings {
	verify_dir "${LAST_DIR}"
	verify_dir "${CATALOG_DIR}"
	verify_dir "${IMG_DIR}"
	verify_dir "${CAT_DIR}"
	verify_dir "${FULL_DIR}"
	if [ ! -d ${LDRAW_DIR} ]; then
		echo LDraw directory not found at ${LDRAW_DIR}
		exit
	fi
}

function scan_parts {
	cd ${LDRAW_DIR}/parts
	if [ -e ${LAST_ALL_PARTS_LIST} ]; then
		cp ${LAST_ALL_PARTS_LIST} ${ALL_PARTS_LIST}
	else
		echo Scanning parts...
		for partFile in *.dat
		do
			scan_part ${partFile}
		done
		echo Sorting parts list...
		# Set locale to UTF-8 for sorting, since LDraw officially uses UTF-8
		export LC_ALL=UTF-8
		sort -f ${ALL_PARTS_LIST} > ${ALL_PARTS_LIST}.tmp
		mv ${ALL_PARTS_LIST}.tmp ${ALL_PARTS_LIST}
		cp ${ALL_PARTS_LIST} ${LAST_ALL_PARTS_LIST}
	fi
	cut -d  -f 1 ${ALL_PARTS_LIST} | sort -u > ${CATEGORIES_LIST}
}

function generate_part_images {
	num_imgs=`ls ${IMG_DIR}/ | wc -l`
	num_parts=`cat ${ALL_PARTS_LIST} | wc -l`
	if [ ${num_imgs} != ${num_parts} ]; then
		echo Generating standard part images...
		create_images ${PARTS_LIST} -cg30,45,275000
		echo Generating panel part images...
		create_images ${PANELS_LIST} -cg30,225,275000
		echo Generating baseplate part images...
		create_images ${BASEPLATES_LIST} -cg30,45,550000
	else
		echo Images already generated.
	fi
}

function print_cat_row {
	echo '		<tr>
			<td><a href="cat/LDraw'${1/ /%20}'.html">'${*}'</a></td>
		</tr>'
}

function create_cat_row {
	print_cat_row "${*}" >> "${CATALOG}"
}

function generate_html {
	echo Generating HTML...
	create_style_sheet
	create_full_header
	create_main_header
	while read line
	do
		create_cat_header "${line}"
		create_cat_row "${line}"
	done < "${CATEGORIES_LIST}"
	create_main_footer
	process_parts
	create_full_footer
	while read line
	do
		create_cat_footer ${line}
	done < "${CATEGORIES_LIST}"
	echo HTML created.
}

function get_cmdline_vars_from_line {
	if [[ "${*}" =~ ([^]*)([^]*) ]]; then
		part_filename=${BASH_REMATCH[1]}
		cmdline=${BASH_REMATCH[2]}
	fi
}

function get_vars_from_line {
	if [[ "${*}" =~ ([^]*)([^]*)([^]*)([^]*)([^]*) ]]; then
		cat=${BASH_REMATCH[1]}
		desc=${BASH_REMATCH[2]}
		part_filename=${BASH_REMATCH[3]}
		cmdline=${BASH_REMATCH[4]}
		keywords=${BASH_REMATCH[5]}
	fi
}

large_parts=$(cat <<EOF
44556
93541
54093
6181
53452
2372c01
2372c01d01
92709
92710
92711
92709c01
92709c02
92709c04
92709c03
65417
65418
20033
18913
18912
20033c01
20033c02
54779
54100
54101
54779c02
54779c01
30215
57786
57915
2372
2373
30072
47116
6161
30477
30401
35199
86501
6490
31012
2842c02
2842c01
2842
2840
23221
74781-f2
74781-f1
74780-f2
74780-f1
2677
2678
2671
2886
2885
2881a
2882
2880a
353
92710c01
92710c02
92710c03
4196
18601
33086
u572p02
u572p01
u9328
u9494
u9494p01
u9499
u9499c01
u9499p01
u9499p01c01
64991
u9495
u9495p01
94318
66645b
66645bp01
61898e
36069b
36069bp01
66645a
66645ap01
36069a
36069ap01
61898d
61898dc01
572c02
99013
99013p01
6391
2972
87058
2869c02
2869c01
866c04
3241ac04
73696c04
73696c03
73696c02
73696c01
73697c04
73697c03
73697c02
73697c01
u9232c02
u9232c01
u9231c02
u9231c01
u9234c02
u9234c01
u9233c02
u9233c01
3229bc04
3229ac04
948ac02
948ac01
949ac02
949ac01
53400c04
2869
u9266
73696c00
73696
864
73697c00
73697
865
u9232c00
u9232
u9220
u9231c00
u9231
u9234c00
u9234
u9221
u9233c00
u9233
948a
949a
948c
948b
949b
EOF
)

# These aren't actually baseplates, but they are too big to fit, so pretend
# that they are so they will be rendered at half scale.
function populate_large_parts {
	touch test.txt
	for part in ${large_parts}; do
		echo ${part}.dat >> ${BASEPLATES_LIST}
	done
}

# Main program
process_command_line ${*}
init_vars
verify_settings
create_empty_lists
populate_large_parts
scan_parts
pre_process_parts
generate_part_images
generate_html
rm -f ${ALL_PARTS_LIST}
