String getIndex(String ID, String TAR, String VER) {
String ret = "<!DOCTYPE html>\n"
"<html>\n"
"  <head> </head>\n"
"  <style>\n"
"    html {\n"
"      max-width: 70ch;\n"
"      padding: calc(1vmin + 0.5rem);\n"
"      margin-inline: auto;\n"
"      font-size: clamp(1em, 0.909em + 0.45vmin, 1.25em);\n"
"      font-family: system-ui;\n"
"    }\n"
"    body :not(:is(h1, h2, h3, h4, h5, h6)) {\n"
"      line-height: 1.75;\n"
"    }\n"
"  </style>\n"
"  <body>\n"
"    <h1>EZCheck Config Tool 1.31</h1>\n"
"    <p>Upon configuration, the module will restart.</p>\n"
"\n"
"    <div>\n"
"      <h3>Set device id</h3>\n"
"      Old id: " + ID + "\n"
"      <br />\n"
"      New id:\n"
"      <input type='text' placeholder='Ex: Laser Cutter' id='idDiv' />\n"
"      <br />\n"
"      Key: <input type='password' id='idKeyDiv' />\n"
"      <br />\n"
"      <button id='setIdDiv' onclick='setId()'>Set</button>\n"
"    </div>\n"
"\n"
"    <div>\n"
"      <h3>Set target url</h3>\n"
"      Old tar: " + TAR + "\n"
"      <br />\n"
"      New tar:\n"
"      <input type='text' placeholder='Ex: https://google.com' id='tarDiv' />\n"
"      <br />\n"
"      Key: <input type='password' id='tarKeyDiv' />\n"
"      <br />\n"
"      <button id='setTarDiv' onclick='setTar'>Set</button>\n"
"    </div>\n"
"\n"
"    <div>\n"
"      <h3>Update device firmware</h3>\n"
"      Old firmware version: " + VER + "\n"
"      <br />\n"
"      <form method='POST' action='#' enctype='multipart/form-data' id='fileDiv'>\n"
"        <input type='file' name='update' />\n"
"      </form>\n"
"      Key: <input type='password' id='fileKeyDiv' />\n"
"      <br />\n"
"      <button id='setFileDiv' onclick='setFile()'>Update</button>\n"
"      <div>Progress: <span id='prg'>0</span>%</div>\n"
"    </div>\n"
"\n"
"    <script>\n"
"      const server = 'http://" + ID + ".local';\n"
"      const idDiv = document.getElementById('idDiv');\n"
"      const idKeyDiv = document.getElementById('idKeyDiv');\n"
"      async function setId() {\n"
"        const res = await fetch(\n"
"          server + '/api/setId/' + idDiv.value + '/' + idKeyDiv.value,\n"
"          {\n"
"            method: 'POST',\n"
"            headers: { 'Content-Type': 'application/json' },\n"
"          }\n"
"        );\n"
"        alert(res.json());\n"
"      }\n"
"      const tarDiv = document.getElementById('tarDiv');\n"
"      const tarKeyDiv = document.getElementById('tarKeyDiv');\n"
"      async function setTar() {\n"
"        const res = await fetch(\n"
"          server + '/api/setTar/' + tarDiv.value + '/' + tarKeyDiv.value,\n"
"          {\n"
"            method: 'POST',\n"
"            headers: { 'Content-Type': 'application/json' },\n"
"          }\n"
"        );\n"
"        alert(res.json());\n"
"      }\n"
"      const fileDiv = document.getElementById('fileDiv');\n"
"      const fileKeyDiv = document.getElementById('fileKeyDiv');\n"
"      function setFile() {\n"
"        const data = new FormData(fileDiv);\n"
"        var xhr = new XMLHttpRequest();\n"
"        xhr.onreadystatechange = function () {\n"
"          if (xhr.readyState == XMLHttpRequest.DONE) {\n"
"            if (xhr.status == 200) {\n"
"              alert(xhr.responseText);\n"
"            } else {\n"
"              alert(xhr.status, 'There was an error with the server');\n"
"            }\n"
"          }\n"
"        };\n"
"        xhr.upload.addEventListener(\n"
"          'progress',\n"
"          function (evt) {\n"
"            if (evt.lengthComputable) {\n"
"              var per = evt.loaded / evt.total;\n"
"              document.getElementById('prg').innerHTML = Math.round(per * 100);\n"
"            }\n"
"          },\n"
"          false\n"
"        );\n"
"        xhr.open('POST', server + '/update/' + fileKeyDiv.value);\n"
"        console.log(xhr);\n"
"        xhr.send(data);\n"
"      }\n"
"    </script>\n"
"  </body>\n"
"</html>\n"
;return ret;}