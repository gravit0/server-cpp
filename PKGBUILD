pkgname=cluservercpp
pkgver=1.0.1
pkgrel=1
pkgdesc="Base TCP Server"
arch=('x86_64' 'i686' 'arm')
license=('LGPL')
depends=("boost-libs" "mysqlpp")
options=('strip')
source=("git+https://Gravit0@bitbucket.org/Gravit0/cluserver-cpp.git")
sha512sums=('SKIP')

build() {
  cd "${srcdir}/cluservercpp"
  cmake .
  make
}

package() {
  cd "${srcdir}/cluservercpp"
  #make install
  mkdir ${pkgdir}/usr/
  mkdir ${pkgdir}/usr/bin/
  cp cluservercpp ${pkgdir}/usr/bin/
}
