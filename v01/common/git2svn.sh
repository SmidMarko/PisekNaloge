FOLDERS=""

echo "Getting 'java-scanner' repository "
rm -rf /tmp/java-scanner
git clone https://github.com/franceioi/java-scanner /tmp/java-scanner
cp /tmp/java-scanner/src/Scanner.java lib/java/algorea/
FOLDERS="$FOLDERS lib/java/algorea/"
echo ""

echo "Getting 'javascool-support' repository "
rm -rf /tmp/javascool-support
git clone https://github.com/franceioi/javascool-support /tmp/javascool-support
cp /tmp/javascool-support/src/trans/FranceIOIJvs2Java.java transformer/javascool/
cp /tmp/javascool-support/src/lib/*.java lib/jvs/javascool/
FOLDERS="$FOLDERS transformer/javascool/ lib/jvs/javascool/"
echo ""

echo "Getting 'lib-sanitizer' repository "
rm -rf /tmp/lib-sanitizer
git clone https://github.com/franceioi/lib-sanitizer /tmp/lib-sanitizer
cp /tmp/lib-sanitizer/src/*.h sanitizer/
FOLDERS="$FOLDERS sanitizer/"
echo ""

echo "Commiting changes"
svn commit -m "Moving git changes to svn" $FOLDERS
echo ""

echo "Done !!"
