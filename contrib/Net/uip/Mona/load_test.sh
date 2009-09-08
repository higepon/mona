#!/bin/bash
AB="ab -r -n 30000 -c 20 http://192.168.50.3/" #localhost:8080/"

${AB}"index.html" 
${AB}"about.html" 
${AB}"control.html"
${AB}"img/bg.png" 
${AB}"404.html" 
${AB}"cgi/files"
${AB}"cgi/stats"
${AB}"cgi/tcp" 
