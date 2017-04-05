# $FreeBSD$
PORTNAME=	ldview
PORTVERSION=	4.2
PORTREVISION=	99
CATEGORIES=	graphics
USE_GITHUB=	yes
GH_ACCOUNT=	tcobbs
GH_TAGNAME=	9ad5ee0
MAINTAINER=	pbartfai@stardust.hu
COMMENT=	LDraw model viewer
WRKSRC_SUBDIR=	QT
USES=		qmake gmake desktop-file-utils shared-mime-info
USE_QT5=	buildtools_build qmake_build
QMAKE_ARGS+=	-spec freebsd-clang MANDIR=${PREFIX}/man/ DOCDIR=${STAGEDIR}${PREFIX}/share/doc/ldview/ SYSCONFDIR=${PREFIX}/etc

pre-configure:
	cd ${WRKSRC} ; lrelease LDView.pro

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
	rm -f ${STAGEDIR}${PREFIX}/share/kde4/services/ldviewthumbnailcreator.desktop
	rmdir ${STAGEDIR}${PREFIX}/share/kde4/services
	rmdir ${STAGEDIR}${PREFIX}/share/kde4
	rm -f ${STAGEDIR}${PREFIX}/share/application-registry/ldview.applications
	rmdir ${STAGEDIR}${PREFIX}/share/application-registry

.include <bsd.port.mk>
