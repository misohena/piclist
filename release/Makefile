TMP_DIR = piclist
ARCHIVE_FILE = piclist.zip
PROGRAM = ../vc10/Release/piclist.exe
DOCUMENTS = $(wildcard ../*.png) $(wildcard ../*.jpg) $(wildcard ../*.org) $(wildcard ../*.html)

$(ARCHIVE_FILE): $(PROGRAM) $(DOCUMENTS)
	-rm -fR $(TMP_DIR)
	-rm -f $(ARCHIVE_FILE)
	mkdir -p piclist
	cp ../vc10/Release/piclist.exe $(TMP_DIR)
	cp $(DOCUMENTS) $(TMP_DIR)
	zip $(ARCHIVE_FILE) -r $(TMP_DIR)
