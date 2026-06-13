<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST');
header('Access-Control-Allow-Headers: Content-Type');

$status_file = __DIR__ . '/status.json';

function readStatus() {
    global $status_file;
    if (file_exists($status_file)) {
        $data = json_decode(file_get_contents($status_file), true);
        if ($data) return $data;
    }
    return [
        'status' => 'DISCONNECTED',
        'is_pressed' => false,
        'steps' => 100,
        'timestamp' => time()
    ];
}

function writeStatus($status, $is_pressed, $steps = null) {
    global $status_file;
    $data = readStatus();
    $data['status'] = $status;
    $data['is_pressed'] = $is_pressed;
    $data['timestamp'] = time();
    if ($steps !== null) {
        $data['steps'] = (int)$steps;
        if ($data['steps'] < 1) $data['steps'] = 1;
        if ($data['steps'] > 255) $data['steps'] = 255;
    }
    file_put_contents($status_file, json_encode($data, JSON_PRETTY_PRINT));
    return $data;
}

function sendJSON($data, $code = 200) {
    http_response_code($code);
    header('Content-Type: application/json; charset=utf-8');
    echo json_encode($data, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
    exit;
}

$request_uri = $_SERVER['REQUEST_URI'];
$request_method = $_SERVER['REQUEST_METHOD'];
$path = parse_url($request_uri, PHP_URL_PATH);

switch ($path) {
    case '/':
    case '/index.html':
        header('Content-Type: text/html; charset=utf-8');
        readfile(__DIR__ . '/index.html');
        break;
    
    case '/status':
        if ($request_method === 'GET') {
            sendJSON(readStatus());
        }
        break;
    
    case '/grip':
    case '/status_on':
        if ($request_method === 'GET') {
            sendJSON(writeStatus('MOVING', true));
        }
        break;
    
    case '/release':
    case '/status_off':
        if ($request_method === 'GET') {
            sendJSON(writeStatus('RELEASED', false));
        }
        break;
    
    case '/connect':
        if ($request_method === 'GET') {
            sendJSON(writeStatus('READY', false));
        }
        break;
    
    case '/disconnect':
        if ($request_method === 'GET') {
            sendJSON(writeStatus('DISCONNECTED', false));
        }
        break;
    
    case '/set_steps':
        if ($request_method === 'GET' && isset($_GET['value'])) {
            $steps = (int)$_GET['value'];
            if ($steps >= 1 && $steps <= 255) {
                $current = readStatus();
                sendJSON(writeStatus($current['status'], $current['is_pressed'], $steps));
            } else {
                sendJSON(['error' => 'Шаги должны быть от 1 до 255'], 400);
            }
        }
        break;
    
    default:
        sendJSON(['error' => 'Страница не найдена'], 404);
        break;
}
?>