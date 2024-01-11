<?php
header("Content-type: text/html\n\n");
echo "\r\n\r\n";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$image_name = $_FILES["work"]["name"];
	$image_tmp = $_FILES["work"]["tmp_name"];
	$image_type = $_FILES["work"]["type"];
	$image_size = $_FILES["work"]["size"];
	move_uploaded_file($image_tmp, "uploads/$image_name");
}
?>

<form action="script.php" method="post" enctype="multipart/form-data">
	<input type="file" name="work">
	<input type="submit" value="Upload">