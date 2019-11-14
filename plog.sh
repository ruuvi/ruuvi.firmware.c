#!/bin/bash
rm -rf issues/fullhtml
rm issues/shorthtml.html
rm issues/PVS-Studio.tasks
rm issues/pvs.xml
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d Renew -t fullhtml -o issues issues/raw/*.PVS-Studio.log
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d Renew -t tasklist -o issues/PVS-Studio.tasks issues/raw/*.PVS-Studio.log
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d Renew -t xml -o      issues/pvs.xml issues/raw/*.PVS-Studio.log
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d Renew -t html -o     issues/shorthtml.html issues/raw/*.PVS-Studio.log
