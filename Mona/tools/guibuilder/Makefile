all:
#	javac -d . src/monaforms/*.java
	javac -d . src/baygui/*.java

run:
	java guibuilder.Main

javadoc:
	-mkdir doc
	javadoc -d ./doc src/baygui/*.java

jar:
	jar -cvmf META-INF/MANIFEST.MF guibuilder.jar guibuilder resources src Makefile
