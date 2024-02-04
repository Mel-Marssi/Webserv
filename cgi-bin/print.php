<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Check Cookies</title>
</head>
<body>

<?php
// Check if cookies are enabled
if (!isset($_COOKIE["testCookie"])) {
    echo "Cookies are not enabled!";
} else {
    echo "Cookies are enabled!";
}
?>

</body>
</html>
