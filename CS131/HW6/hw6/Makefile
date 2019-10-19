.SILENT:

default:
	kotlinc everyNth.kt test.kt -include-runtime -d everyNth.jar

check: default
	java -jar everyNth.jar

dist: default
	tar -czvf hw6.tar.gz everyNth.jar everyNth.kt test.kt Makefile

clean:
	rm -rf everyNth.jar hw6.tar.gz
