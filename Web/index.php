<?php
// Put here the path to KWStyle
//$kwstyle = "/www/CADDLab/kwstyle/KWStyle";
$kwstyle = "/projects/KWStyle/KWStyle-Linux/KWStyle";

//$kwstyle2 = "/www/CADDLab/kwstyle/";
$kwstyle2 = "/projects/KWStyle/KWStyle/Web/";

Header_POST["Header"];
$xmlfile = $_POST["xmlfile"];
$xmlfileselect = $_GET["xmlfileselect"];

if($_FILES['userfile']['name'])
{
$newloc = "/tmp/".$_FILES['userfile']['name'];
move_uploaded_file($_FILES['userfile']['tmp_name'],$newloc);
$command = $kwstyle." -xml ";
$command .= $xmlfile;
$command .= " -exporthtml ";
$command .= $newloc;
//echo $command;
$output = shell_exec($command);
echo $output;
} // end submit

 ?>
 
<SCRIPT LANGUAGE="Javascript">
function test(value)
{
  url = "index.php?xmlfileselect="+value;
	window.location.href = url;
}
</SCRIPT>

<?php
$currentfile = "";
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html><head><style type="text/css">
</style><title>KWStyle - The Source Code Checker</title>
<meta name="keywords" content="Style, Checker, Source, Code, Kitware, Julien, Jomier">
<meta name="description" content="The Source Code Checker">
<meta name="rating" content="General">
<meta name="ROBOTS" content="ALL">
<link rel="stylesheet" href="default.css" type="text/css">
<style type="text/css">
<!--
.style7 {font-size: 12px}
-->
</style>
</head>
<link rel="icon" href="/KWStyle/favicon.ico" type="image/ico">
<link rel="SHORTCUT ICON" href="/KWStyle/favicon.ico">

<body topmargin="0" leftmargin="0" alink="#000000" bgcolor="#ffffff" link="#0000ff" marginheight="0" marginwidth="0" text="black" vlink="#000000">
<img src="images/TitleBar.jpg" width="650" height="170"> 
<table width="100%" border="0">
  <tr>
    <td width="13%" valign="top"> 
      <table style="background-image: 
     url('images/CurvedSideBar.jpg'); background-repeat:no-repeat; 
       border: 0 ridge #800000" border="0" cellpadding="4" cellspacing="0" width="143">
        <tbody> 
        <tr> 
          <td width="135" height="369" valign="top"> <br>
            <br>
            <br>
            <br>
            <br>
            <a class="left" href="index.htm"><b>&nbsp;Home</b></a><br>
            <br>
            <a class="left" href="features.htm"><b>&nbsp;Features</b></a><br>
			<br>
			<a class="left" href="http://192.168.113.30/KWStyle"><b>&nbsp;Check my file</b></a><br>
            <br>
            <a class="left" href="download.htm"><b>&nbsp;Download</b></a><br>
            <br>
            <a class="left" href="example/KWSMatrix.html"><b>&nbsp;Example</b></a><br>
            <br>
            <a class="left" href="testing.htm"><b>&nbsp;Testing</b></a><br>
            <br>
            <a class="left" href="sponsors.htm"><b>&nbsp;Sponsors</b></a><br>
            <br>
            <a class="left" href="documentation.htm"><b>&nbsp;Documentation</b></a><br>
            <br>
            <a class="left" href="copyright.htm"><b>&nbsp;Copyright</b></a><br>
            <br>
            <a class="left" href="http://public.kitware.com/Bug/"><b>&nbsp;Bug 
            Tracker</b></a><br>
            <br>
          </td>
        </tr>
        </tbody>
      </table>
    </td>
    <td width="87%" valign="top"> 
      <!-- The data encoding type, enctype, MUST be specified as below -->
<form enctype="multipart/form-data" action=<?php echo $_SERVER['PHP_SELF'];?> method="POST">
    <!-- MAX_FILE_SIZE must precede the file input field -->
    <p>
      Select a predefined style: 
      <select ONCHANGE="test(this.options[this.selectedIndex].value)" name="xmlfile" id="xmlfile">
        <?php
    				// create a handler for the directory
						
    				$handler = opendir($kwstyle2);

						$i = 0;

    				// keep going until all files in directory have been read
   					while ($file = readdir($handler)) 
						  {
        			// if $file isn't this directory or its parent, 
        			// add it to the results array
       				 if (strstr($file,"kws.xml"))
							   {
								 if($i==0)
								   {
									 $currentfile = $file;
									 }
								 $name = substr($file,0,strlen($file)-8);
								 echo "<option value=".$file;
								 if(strstr($file,$xmlfileselect)) // select the last revision
								   {
									 $currentfile = $file;
								   echo(" selected");
								   }
								 echo ">";
								 echo $name;
								 echo "</option>";
								 $i++;
                }
							
    					}
    closedir($handler);
		?>
      </select>
      <br>
      <br>
      <?php
			
class xmlParser{
   var $xml_obj = null;
   var $output = array();
   var $attrs;

   function xmlParser(){
       $this->xml_obj = xml_parser_create();
       xml_set_object($this->xml_obj,$this);
       xml_set_character_data_handler($this->xml_obj, 'dataHandler');
       xml_set_element_handler($this->xml_obj, "startHandler", "endHandler");
   }

   function parse($path){
       if (!($fp = fopen($path, "r"))) {
           die("Cannot open XML data file: $path");
           return false;
       }

       while ($data = fread($fp, 4096)) {
           if (!xml_parse($this->xml_obj, $data, feof($fp))) {
               die(sprintf("XML error: %s at line %d",
               xml_error_string(xml_get_error_code($this->xml_obj)),
               xml_get_current_line_number($this->xml_obj)));
               xml_parser_free($this->xml_obj);
           }
       }

       return true;
   }

   function startHandler($parser, $name, $attribs){
       $_content = array();
       $_content['attrs'] = $name;
			 array_push($this->output, $_content);
   }

   function dataHandler($parser, $data){
       if($data!="\n") {
           $_output_idx = count($this->output) - 1;
					// echo $data."<br>";
           $this->output[$_output_idx]['content'] = $data;
       }
   }

   function endHandler($parser, $name){ }
	 }

$p =& new xmlParser();
$p->parse($currentfile);
$a = $p->output;

for($i=1;$i<count($a);$i++)
{
  echo "<b>".ucfirst(strtolower($a[$i]['attrs']))." : </b>".$a[$i]['content']."<br>";
} 
?>
    </p>
 <p>    <input type="hidden" name="MAX_FILE_SIZE" value="30000" />
    <!-- Name of input element determines name in $_FILES array -->
    Check this file: 
		 <input name="userfile" type="file" size="70" />
    </p>
    <p>      
      <input type="submit" value=">> Check Style" />
  </p>
</form>
</td>
  </tr>
</table>
</body></html>

