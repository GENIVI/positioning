---------------------------------
Document Generation
---------------------------------

0)
Install the following packages:
sudo apt-get install doxygen
sudo apt-get install pdftk

1) 
Modify the <component name>API-part1.doc as needed.

2)
Convert the <component name>API-part1.doc
into a PDF document with the same name.

3) 
Navigate to the directory 'tools'.

4) 
Generate the documentation:
./generate-doc.sh

A document called <component name>API-part2.pdf will be automatically generated.
The final component documentation (<component name>API.pdf) will be created in 
the parent directory.
