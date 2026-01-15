//
//  LargeConsts.cpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/6/25.
//

#include "Consts.h"

const StringSet Consts::largeParts = {
	"44556",
	"93541",
	"54093",
	"6181",
	"53452",
	"2372c01",
	"2372c01d01",
	"92709",
	"92710",
	"92711",
	"92709c01",
	"92709c02",
	"92709c04",
	"92709c03",
	"65417",
	"65418",
	"20033",
	"18913",
	"18912",
	"20033c01",
	"20033c02",
	"54779",
	"54100",
	"54101",
	"54779c02",
	"54779c01",
	"30215",
	"57786",
	"57915",
	"2372",
	"2373",
	"30072",
	"47116",
	"6161",
	"30477",
	"30401",
	"35199",
	"86501",
	"6490",
	"31012",
	"2842c02",
	"2842c01",
	"2842",
	"2840",
	"23221",
	"74781-f2",
	"74781-f1",
	"74780-f2",
	"74780-f1",
	"2677",
	"2678",
	"2671",
	"2886",
	"2885",
	"2881a",
	"2882",
	"2880a",
	"353",
	"92710c01",
	"92710c02",
	"92710c03",
	"4196",
	"18601",
	"33086",
	"u572p02",
	"u572p01",
	"u9328",
	"u9494",
	"u9494p01",
	"u9499",
	"u9499c01",
	"u9499p01",
	"u9499p01c01",
	"64991",
	"u9495",
	"u9495p01",
	"94318",
	"66645b",
	"66645bp01",
	"61898e",
	"36069b",
	"36069bp01",
	"66645a",
	"66645ap01",
	"36069a",
	"36069ap01",
	"61898d",
	"61898dc01",
	"572c02",
	"99013",
	"99013p01",
	"6391",
	"2972",
	"87058",
	"2869c02",
	"2869c01",
	"866c04",
	"3241ac04",
	"73696c04",
	"73696c03",
	"73696c02",
	"73696c01",
	"73697c04",
	"73697c03",
	"73697c02",
	"73697c01",
	"u9232c02",
	"u9232c01",
	"u9231c02",
	"u9231c01",
	"u9234c02",
	"u9234c01",
	"u9233c02",
	"u9233c01",
	"3229bc04",
	"3229ac04",
	"948ac02",
	"948ac01",
	"949ac02",
	"949ac01",
	"53400c04",
	"2869",
	"u9266",
	"73696c00",
	"73696",
	"864",
	"73697c00",
	"73697",
	"865",
	"u9232c00",
	"u9232",
	"u9220",
	"u9231c00",
	"u9231",
	"u9234c00",
	"u9234",
	"u9221",
	"u9233c00",
	"u9233",
	"948a",
	"949a",
	"948c",
	"948b",
	"949b",
	"35011",
	"35011c01",
};

const StringVector Consts::panelPrefixes1 = {
	"4864",
	"6268",
	"4215",
	"2362",
	"4865",
};

const StringVector Consts::panelPrefixes2 = {
	"4345ap",
	"4345bp",
};

const std::string Consts::styleSheet = R"!RAW!(body {
	font-family: tahoma, sans-serif;
}
p {
	margin-top: 0px;
	margin-bottom: 0px;
}
div {
	position: absolute;
}
table {
	border-collapse: collapse;
	border: 2px solid #000000;
	background-color: #FFFFFF;
	padding: 0px;
}
tr {
	page-break-inside: avoid;
}
th {
	background-color: #0080D0;
	color: #FFFFFF;
	border-bottom: 1px solid #000000;
	border-right: 1px solid #00558A;
	padding: 4px 8px;
}
th.title {
	background-color: #EEEEEE;
	color: #000000;
	font-size: 150%;
}
td {
	background-color: #FFFFFF;
	border-bottom: 1px solid #BFBFBF;
	border-right: 1px solid #BFBFBF;
	padding: 2px 4px;
}
td.label {
	text-align: right;
	border-right: 0px;
	width: 1%;
	padding: 2px 0px 2px 4px;
}
table.credits {
	border-collapse: collapse;
	border-style: none;
	background-color: transparent;
	margin: 0px;
	width: 100%;
}
td.credits {
	background-color: #EEEEEE;
	border-style: none;
	padding: 0px;
}
table.credits td {
	background-color: transparent;
	color: #808080;
	border-style: none;
	font-size: 65%;
}
a {
	text-decoration: none;
	border-style: none;
}
a:link {
	color: #000080;
}
a:visited {
	color: #0000FF;
}
a:hover {
	text-decoration: underline;
	color: #000080;
}
img {
	float: right;
}
a img {
	border-style: none;
}
:link:hover img {
	background-color: #D0E8FF;
}
:visited:hover img {
	background-color: #A0C0FF;
}
)!RAW!";

const std::string Consts::htmlHeader = R"!RAW!(<!DOCTYPE html>
<html>
<head>
<title>${title}</title>
<style type="text/css" title="Standard"><!--
${style_sheet}
--></style>
<!--<link href="'${css_path}${css_filename}'" title="Standard" rel="stylesheet" type="text/css">-->
</head>
<body>
)!RAW!";

const std::string Consts::mainPrefix = R"!RAW!(<table>
	<thead>
		<tr>
			<th>Category</th>
		</tr>
	</thead>
	<tbody>
)!RAW!";

const std::string Consts::htmlSuffix = R"!RAW!(	</tbody>
</table>
</body>
</html>
)!RAW!";

const std::string Consts::categoryRow = R"!RAW!(		<tr>
			<td><a href="cat/LDraw${category_file}.html">${category}</a></td>
		</tr>
)!RAW!";

const std::string Consts::catPrefix = R"!RAW!(<table>
	<thead>
		<tr>
			<th colspan="3">${category}</th>
		</tr>
	</thead>
	<tbody>
)!RAW!";

const std::string Consts::partRow = R"!RAW!(		<tr valign="top">
			<td rowspan="${rowspan}" align="right"><img width="${png_width}" height="${png_height}" src="../img/${png_filename}" alt="Part Image"/></td>
			<td class="label" align="right" height="99%"><font size="-2">File</font></td><td>${part_filename}</td>
		</tr>
		<tr valign="top"><td class="label" align="right"><font size="-2">Name</font></td><td>${desc}</td></tr>
)!RAW!";

const std::string Consts::keywordsRow = R"!RAW!(		<tr valign="top"><td class="label" align="right"><font size="-2">Keywords</font></td><td>${keywords}</td></tr>
)!RAW!";
