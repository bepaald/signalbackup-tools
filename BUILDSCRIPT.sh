#!/bin/sh
set -o errexit
set -o nounset
set -o pipefail

COMPILER=$(which g++)
NUMCPU=$(nproc || echo 1)

BUILD_CMD="$COMPILER -std=c++2a -c -Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -pedantic -fomit-frame-pointer -O3 -march=native -flto"
MODULES="cryptbase/cryptbase.cc
cryptbase/getbackupkey.cc
cryptbase/getcipherandmac.cc
endframe/statics.cc
filedecryptor/getframe.cc
filedecryptor/initbackupframe.cc
filedecryptor/getattachment.cc
filedecryptor/filedecryptor.cc
arg/arg.cc
sharedprefframe/statics.cc
avatarframe/statics.cc
attachmentframe/statics.cc
fileencryptor/encryptattachment.cc
fileencryptor/encryptframe.cc
fileencryptor/fileencryptor.cc
sqlstatementframe/statics.cc
sqlstatementframe/buildstatement.cc
signalbackup/signalbackup.cc
main.cc
backupframe/init.cc
sqlitedb/sqlitedb.cc
databaseversionframe/statics.cc
headerframe/statics.cc
stickerframe/statics.cc"

MODULE_COUNT=$(wc -l <<<"$MODULES")

i=0
while read module; do
  i=$((i+1))
  outdir="$(dirname $module)/o"
  module_name=$(basename $module)
  outfile="$outdir/${module_name%.cc}.o"

  echo "BUILDING ($i/$MODULE_COUNT): $module_name -> $outfile"
  mkdir -p $outdir
  $BUILD_CMD -o "$outfile" "$module"
  modules_out="${modules_out:-} $outfile"
done <<<"$MODULES"

echo "LINKING: signalbackup-tools"
echo g++ -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU $modules_out -lcryptopp -lsqlite3 -o "signalbackup-tools"
g++ -Wall -Wextra -Wl,-z,now -O3 -s -flto=$NUMCPU $modules_out -lcryptopp -lsqlite3 -o "signalbackup-tools"
