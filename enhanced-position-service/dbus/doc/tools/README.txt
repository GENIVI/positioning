---------------------------------
Document Generation
---------------------------------

0)
Install the following packages:
sudo apt-get install wkhtmltopdf
sudo apt-get install xsltproc
sudo apt-get install pdftk

1) 
Modify the <component specification name>-part1.doc as needed.

2)
Convert the <component specification name>-part1.doc
into a pdf document with the same name.

3) 
Navigate to the directory 'tools'.

4) 
Generate the documentation:
./generate-doc.sh

A document called <component specification name>-part2.pdf will be automatically generated.
The final component documentation (<component specification name>.pdf) will be created in 
the parent directory.


------------
Know Issues
------------
Should you get the following error message 'Error: This version of wkhtmltopdf is build against an unpatched version of QT, and does not support more then one input document', 
remove the 'wkhtmltopdf' package using the command: sudo apt-get purge wkhtmltopdf
Then download and install the debian package 'wkhtmltopdf' from the following site: http://wkhtmltopdf.org/downloads.html
