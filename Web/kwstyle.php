<?php
// Put here the path to KWStyle
$kwstyle = "/projects/KWStyle/KWStyle-Linux/KWStyle";

Header_POST["Header"];

if($_FILES['userfile']['name'])
{
//echo "Here = ".$_FILES['userfile']['name']." === ".$_FILES['userfile']['tmp_name'];
$newloc = "/tmp/".$_FILES['userfile']['name'];
move_uploaded_file($_FILES['userfile']['tmp_name'],$newloc);
$command = $kwstyle." -exporthtml ";
$command .= $newloc;
echo $command;
$output = shell_exec($command);
echo $output;
} // end submit
 ?>
<!-- The data encoding type, enctype, MUST be specified as below -->
<form enctype="multipart/form-data" action=<?php echo $_SERVER['PHP_SELF'];?> method="POST">
    <!-- MAX_FILE_SIZE must precede the file input field -->
    <p>
    <input type="hidden" name="MAX_FILE_SIZE" value="30000" />
    <!-- Name of input element determines name in $_FILES array -->
    Check this file: 
    <input name="userfile" type="file" />
  </p>
    <p>
      <input type="checkbox" name="Header" value="checkbox">
Check Header </p>
    <p>      
      <input type="submit" value=">> Check Style" />
            </p>
</form>
