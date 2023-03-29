# Ensure we exit correctly if an error occurs
set -o errexit

### Transformers compilation ###
# Java's Cool ==> Java
echo "*** Compiling JVS=>Java transformer ***"
pushd transformer/javascool/
gcj --encoding=utf8 --main=FranceIOIJvs2Java -o FranceIOIJvs2Java FranceIOIJvs2Java.java
popd
