# $FreeBSD$
PORTNAME=	ldview
PORTVERSION=	4.6
PORTREVISION=	1
CATEGORIES=	graphics
USE_GITHUB=	yes
GH_ACCOUNT=	tcobbs
GH_TAGNAME=	HEAD
MAINTAINER=	pbartfai@stardust.hu
COMMENT=	LDraw model viewer
WRKSRC_SUBDIR=	QT
LIB_DEPENDS=	libOSMesa.so:graphics/osmesa \
		libtinyxml.so:textproc/tinyxml \
		libgl2ps.so:print/gl2ps \
		libminizip.so:archivers/minizip \
		lib3ds.so:graphics/lib3ds
USES=		qt:6 qmake gmake desktop-file-utils shared-mime-info
USE_QT=	base
QMAKE_ARGS+=	-spec freebsd-clang MANDIR=${PREFIX}/man/ DOCDIR=${STAGEDIR}${PREFIX}/share/doc/ldview/ SYSCONFDIR=${PREFIX}/etc

pre-configure:
	cd ${WRKSRC} ; /usr/local/lib/qt6/bin/lrelease LDView.pro

post-build:
	cd ${WRKSRC} ; cd ../OSMesa ; gmake

post-install:
	install -m 755 ${WRKSRC}/../OSMesa/ldview ${STAGEDIR}${PREFIX}/bin/ldview
	install -m 644 ${WRKSRC}/../OSMesa/ldview.1 ${STAGEDIR}${PREFIX}/man/man1/
	gzip -9f ${STAGEDIR}${PREFIX}/man/man1/ldview.1
	install -m 644 ${WRKSRC}/../OSMesa/ldviewrc.sample ${STAGEDIR}${PREFIX}/share/ldview/


post-stage:
	rm -f ${STAGEDIR}${PREFIX}/share/mime-info/ldraw.mime
	rm -f ${STAGEDIR}${PREFIX}/share/mime-info/ldraw.keys
	rmdir ${STAGEDIR}${PREFIX}/share/mime-info
	rm -f ${STAGEDIR}${PREFIX}/share/application-registry/ldview.applications
	rmdir ${STAGEDIR}${PREFIX}/share/application-registry

.include <bsd.port.mk>
