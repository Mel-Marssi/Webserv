
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Set Cookie</title>
</head>
<body>

<?php
// Check if the form is submitted
if ($_SERVER["REQUEST_METHOD"] == "GET") {
	setcookie("username",$_GET["username"], time() + (3000));
    echo "Cookie set successfully!";
}
?>

<h2>Set Cookie Form</h2>
<form method="get" action="<?php echo $_SERVER['REQUEST_URI']; ?>">
    <label for="username">Enter your name:</label>
    <input type="text" id="username" name="username" required>
    <button type="submit">Set Cookie</button>
</form>

</body>
</html>
