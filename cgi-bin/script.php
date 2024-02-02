<?php
header("Content-type: text/html\n\n");
echo "\r\n\r\n";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	$image_name = strtolower($_FILES["work"]["name"]);
	$image_tmp = $_FILES["work"]["tmp_name"];
	$image_type = $_FILES["work"]["type"];
	$image_size = $_FILES["work"]["size"];
	$image_error = $_FILES["work"]["error"];
	if ($image_error > 0) {
		echo "Error: " . $image_error . "<br />";
	}
	move_uploaded_file($image_tmp, "uploads/$image_name");
}
?>