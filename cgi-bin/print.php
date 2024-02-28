#!/usr/bin/php-cgi
<?php

// Read the POST data
$postData = file_get_contents('php://input');

// Parse POST data
parse_str($postData, $formData);

// Access the submitted message
$message = $formData['message'];

// Generate response
$response = "You submitted: " . $message;

// Send headers
header("Content-Type: text/plain");
header("Content-Length: " . strlen($response));
header("Connection: close");

// Send the response
echo $response;
?>
