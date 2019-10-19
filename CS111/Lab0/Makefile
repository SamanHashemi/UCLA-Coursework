sub_files = lab0.c Makefile README *.png
.SILENT:

default:
	gcc -o lab0 -Wall -Wextra -g lab0.c

check: default pass in_fail out_fail matching seg_catch

clean:
	rm -f *.o lab0 *.txt *tar.gz

dist: default
	tar -czf lab0-904903562.tar.gz $(sub_files)

pass:
	echo "Testing" > in.txt; \
	./lab0 -i in.txt -o out.txt; \
	if [[ $$? -ne 0 ]]; then \
	echo "Error: Regular input-output failed, check source code"; \
	else \
	echo "'pass' succeeded"; \
	fi; \
	rm -f in.txt out.txt
in_fail:
	echo "Testing" > in.txt; \
	chmod u-r in.txt; \
	echo | ./lab0 -i in.txt -o &>/dev/null;\
	if [[ $$? -ne 1 ]]; then \
	echo "Error: Read lock ignored, check permsission access in code"; \
	else \
	echo "'in_fail' succeeded"; \
	fi; \
	rm -f in.txt
out_fail:
	echo "Testing" > temp.txt; \
	touch tempout.txt; \
	chmod u-w tempout.txt; \
	./lab0 -i temp.txt -o tempout.txt &>/dev/null; \
	if [[ -s "tempout.txt" ]]; then \
	echo "Error: Write permission block ignored 'out_fail' failed during make"; \
	else \
	echo "'out_fail' succeeded"; \
	fi; \
	rm -f temp.txt tempout.txt

matching:
	echo "Working" > temp.txt; \
	./lab0 -i temp.txt -o tempout.txt &>/dev/null; \
	cmp temp.txt tempout.txt; \
	if [[ $$? -ne 0 ]]; then \
	echo "Error: the file values do not match. 'matching' error"; \
	else \
	echo "'matching' succeeded"; \
	fi; \
	rm -f temp.txt tempout.txt

seg_catch:
	echo "Testing" > in.txt
	./lab0 -sc -i in.txt -o out.txt; \
	if [[ -s "out.txt" ]]; then \
	echo "Error: 'catch' failed"; \
	else \
	echo "'catch' succeeded"; \
	fi;