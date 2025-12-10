#!/usr/bin/expect -f
# 05
# a) check that the book is deleted from index
## b) check that list of deleted books is created
##     and file with list is saved
## c) check add after delete with the different strategies

set timeout -1
set programName "library"
set filename "test"

# Borrar ficheros previos
spawn rm -f $filename.db $filename.ind

# Ejecutar programa (first_fit)
spawn ./$programName first_fit test
expect "Type command and argument/s."
expect "exit"


send "add 1001|978-0-11111111-1|Estructuras de Datos Avanzadas|Pearson\r"
expect "Record with BookID=1001 has been added to the database"
expect "exit"

send "add 1002|978-0-22222222-2|Álgebra Lineal|Springer\r"
expect "Record with BookID=1002 has been added to the database"
expect "exit"

send "printInd\n"
expect "Entry #0"
expect "    key: #1001"
expect "    offset: #0"
expect "    size: #58"
expect "Entry #1"
expect "    key: #1002"
expect "    offset: #66"
expect "    size: #44"
expect "exit"

send "add 1003|978-0-33333333-3|Programación en C|McGraw-Hill\r"
expect "Record with BookID=1003 has been added to the database"
expect "exit"

send "add 1004|978-0-44444444-4|Estructuras Discretas|Oxford\r"
expect "Record with BookID=1004 has been added to the database"
expect "exit"

send "add 1005|978-0-55555555-5|Bases de Datos|Pearson\r"
expect "Record with BookID=1005 has been added"
expect "exit"

send "printInd\n"
expect "Entry #0"
expect "    key: #1001"
expect "    offset: #0"
expect "    size: #58"
expect "Entry #1"
expect "    key: #1002"
expect "    offset: #66"
expect "    size: #44"
expect "Entry #2"
expect "    key: #1003"
expect "    offset: #118"
expect "    size: #50"
expect "Entry #3"
expect "    key: #1004"
expect "    offset: #176"
expect "    size: #48"
expect "Entry #4"
expect "    key: #1005"
expect "    offset: #232"
expect "    size: #42"
expect "exit"


send "del 1002\r"
expect "Record with BookID=1002 has been deleted"
expect "exit"

send "del 1004\r"
expect "Record with BookID=1004 has been deleted"
expect "exit"

send "del 1001\r"
expect "Record with BookID=1001 has been deleted"
expect "exit"

send "printInd\n"
expect "Entry #0"
expect "    key: #1003"
expect "    offset: #118"
expect "    size: #50"
expect "Entry #1"
expect "    key: #1005"
expect "    offset: #232"
expect "    size: #42"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #66"
expect "    size: #44"
expect "Entry #1"
expect "    offset: #176"
expect "    size: #48"
expect "Entry #2"
expect "    offset: #0"
expect "    size: #58"
expect "exit"


send "add 1006|978-0-66666666-6|Algoritmos|Springer\r"
expect "Record with BookID=1006 has been added"
expect "exit"

send "printInd\n"
expect "Entry #0"
expect "    key: #1003"
expect "    offset: #118"
expect "    size: #50"
expect "Entry #1"
expect "    key: #1005"
expect "    offset: #232"
expect "    size: #42"
expect "Entry #2"
expect "    key: #1006"
expect "    offset: #66"
expect "    size: #39"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #105"
expect "    size: #5"
expect "Entry #1"
expect "    offset: #176"
expect "    size: #48"
expect "Entry #2"
expect "    offset: #0"
expect "    size: #58"
expect "exit"



send "add 1007|978-0-88888888-8|Inteligencia Artificial|Ediciones\r"
expect "Record with BookID=1007 has been added"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #105"
expect "    size: #5"
expect "Entry #1"
expect "    offset: #176"
expect "    size: #48"
expect "Entry #2"
expect "    offset: #53"
expect "    size: #5"
expect "exit"


send "add 1008|978-0-77777777-7|Matemáticas|Cambridge\r"
expect "Record with BookID=1008 has been added"
expect "exit"

send "printInd\n"
expect "Entry #0"
expect "    key: #1003"
expect "    offset: #118"
expect "    size: #50"
expect "Entry #1"
expect "    key: #1005"
expect "    offset: #232"
expect "    size: #42"
expect "Entry #2"
expect "    key: #1006"
expect "    offset: #66"
expect "    size: #39"
expect "Entry #3"
expect "    key: #1007"
expect "    offset: #0"
expect "    size: #53"
expect "Entry #4"
expect "    key: #1008"
expect "    offset: #176"
expect "    size: #42"
expect "exit"

send "printLst\n"
expect "Entry #0"
expect "    offset: #105"
expect "    size: #5"
expect "Entry #1"
expect "    offset: #218"
expect "    size: #6"
expect "Entry #2"
expect "    offset: #53"
expect "    size: #5"
expect "exit"

send "exit\n"
expect "all done"

puts "\n***************************************************"
puts " TEST PASSED: FIRST-FIT reuse works perfectly!"
puts "***************************************************"
