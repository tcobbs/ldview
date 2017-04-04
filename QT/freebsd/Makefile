# $FreeBSD$
PORTNAME=	ldview
PORTVERSION=	4.2
PORTREVISION=	99
CATEGORIES=	graphics
USE_GITHUB=	yes
GH_ACCOUNT=	tcobbs
GH_TAGNAME=	3607b7a6
MAINTAINER=	pbartfai@stardust.hu
COMMENT=	LDraw model viewer
WRKSRC_SUBDIR=	QT
USES=		qmake gmake desktop-file-utils shared-mime-info
USE_QT5=	buildtools_build qmake_build
QMAKE_ARGS+=	-spec freebsd-clang MANDIR=${PREFIX}/man/ DOCDIR=${STAGEDIR}${PREFIX}/share/doc/ldview/ SYSCONFDIR=${PREFIX}/etc
MAKE_ARGS+=	CC=c++ TESTING=-I/usr/local/include

pre-configure:
	cd ${WRKSRC} ; lrelease LDView.pro

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
