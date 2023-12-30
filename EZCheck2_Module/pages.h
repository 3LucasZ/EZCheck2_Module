const char* aboutHtml = 
"<!DOCTYPE html>\n"
"<html>\n"
"  <head> </head>\n"
"  <body>\n"
"    <h1>About</h1>\n"
"    <p>Content in progress</p>\n"
"  </body>\n"
"</html>\n";
const char* indexHtml = 
"<!DOCTYPE html>\n"
"<html>\n"
"  <head> </head>\n"
"  <body>\n"
"    <h1>Welcome to EZCheck Config!</h1>\n"
"    <p>Content in progress</p>\n"
"  </body>\n"
"</html>\n";
const char* otaHtml = 
"<!DOCTYPE html>\n"
"<html>\n"
"  <head> </head>\n"
"  <body>\n"
"    <h1>EZCheck OTA Updater</h1>\n"
"    <form\n"
"      method='POST'\n"
"      action='#'\n"
"      enctype='multipart/form-data'\n"
"      id='upload_form'\n"
"    >\n"
"      <input type='file' name='update' />\n"
"      <input type='submit' value='Update' />\n"
"    </form>\n"
"    <div id='prg'>progress: 0%</div>\n"
"    <script>\n"
"      const form = document.getElementById('upload_form');\n"
"      form.addEventListener('submit', (e) => {\n"
"        e.preventDefault();\n"
"        const data = new FormData(form);\n"
"        var xhr = new XMLHttpRequest();\n"
"        xhr.onreadystatechange = function () {\n"
"          if (xhr.readyState == XMLHttpRequest.DONE) {\n"
"            if (xhr.status == 200) {\n"
"              document.getElementById('myDiv').innerHTML = xhr.responseText;\n"
"            } else if (xhr.status == 400) {\n"
"              alert('There was an error 400');\n"
"            } else {\n"
"              alert('something else other than 200 was returned');\n"
"            }\n"
"          }\n"
"        };\n"
"        xhr.upload.addEventListener(\n"
"          'progress',\n"
"          function (evt) {\n"
"            if (evt.lengthComputable) {\n"
"              var per = evt.loaded / evt.total;\n"
"              document.getElementById('prg').innerHTML =\n"
"                'progress: ' + Math.round(per * 100) + '%';\n"
"            }\n"
"          },\n"
"          false\n"
"        );\n"
"        xhr.open('POST', '/update');\n"
"        console.log(xhr);\n"
"        xhr.send(data);\n"
"      });\n"
"    </script>\n"
"  </body>\n"
"</html>\n";
