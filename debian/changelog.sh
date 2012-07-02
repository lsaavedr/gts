GTS_VERSION=`grep GTS_MAJOR_VERSION config.h | awk '{print $3;}'`.`grep GTS_MINOR_VERSION config.h | awk '{print $3;}'`.`grep GTS_MICRO_VERSION config.h | awk '{print $3;}'`

version=`date +%y%m%d`
date=`date +"%a, %e %b %Y %T %z"`
cat <<EOF > debian/changelog
gts-snapshot ($GTS_VERSION-$version) jaunty; urgency=low

  * gts-snapshot release (based on Ruben Molina's official debian)

 -- Stephane Popinet <popinet@users.sf.net>  $date
EOF
