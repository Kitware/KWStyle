<?php
// Put here the path to KWStyle
$kwstyle = "/www/CADDLab/kwstyle/KWStyle";
//$kwstyle = "/projects/KWStyle/KWStyle-Linux/KWStyle";
$kwstyle2 = "/www/CADDLab/kwstyle/";
//$kwstyle2 = "/projects/KWStyle/KWStyle/Web/";

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
