# $FreeBSD
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
USES=		qmake gmake
USE_QT5=	buildtools_build qmake_build
QMAKE_ARGS+=	-spec freebsd-clang MANDIR=${PREFIX}/man/ DOCDIR=${STAGEDIR}${PREFIX}/share/doc/ldview/
MAKE_ARGS+=	CC=c++ TESTING=-I/usr/local/include
pre-configure:
	cd ${WRKSRC} ; lrelease LDView.pro
.include <bsd.port.mk>
