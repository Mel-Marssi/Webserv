<?php

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	if (isset($_FILES["fileInput"]))
	{
		$image_name = strtolower($_FILES["fileInput"]["name"]);
		$image_tmp = $_FILES["fileInput"]["tmp_name"];
		$image_type = $_FILES["fileInput"]["type"];
		$image_size = $_FILES["fileInput"]["size"];
		$image_error = $_FILES["fileInput"]["error"];
		if ($image_error > 0) {
			echo "Error: " . $image_error . "<br />";
		}
		move_uploaded_file($image_tmp, "multiplexing/$image_name");
	}
 	else {
		echo "No file uploaded ";
	}
}
?>
