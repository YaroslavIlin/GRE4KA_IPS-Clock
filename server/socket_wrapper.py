import socket
from threading import Thread
from typing import Callable, List

class BaseSocket:
	def _receive_data(self, sender: socket.socket) -> str:
		message_length = int.from_bytes(self.__receive_chunked(sender, 2), 'big')
		return self.__receive_chunked(sender, message_length).decode('utf-8')

	def __receive_chunked(self, sender: socket.socket, to_receive: int) -> bytes:
		received = b''
		while len(received) < to_receive:
			chunk = sender.recv(min(to_receive - len(received), 2048))
			if chunk == b'':
				raise RuntimeError('Connection lost')
			received += chunk

		return received

	def _send_data(self, receiver: socket.socket, data: str) -> None:
		data = bytes(data, 'utf-8')
		receiver.send(len(data).to_bytes(2, 'big'))
		receiver.send(data)

class Server(BaseSocket, Thread):
	@property
	def is_client_connected(self) -> bool:
		return self.__is_client_connected

	@property
	def is_alive(self) -> bool:
		return self.__keep_alive

	def __init__(self, port: int):
		super().__init__()
		self.port = port
		self.__is_client_connected = False
		self.__keep_alive = True
		self.__listeners: List[Callable[[str], None]] = []

	def run(self) -> None:
		try:
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.sock.bind(('', self.port))
			self.__wait_client()
		except Exception as e:
			self.stop()
			print(e)

	def client_listener(self, handler: Callable[[str], None]) -> Callable[[str], None]:
		self.__listeners.append(handler)
		return handler

	def stop(self) -> None:
		self.__keep_alive = False
		self.close_connection()
		print('Server closed')

	def close_connection(self, exception: Exception | str='Connection closed') -> None:
		if self.is_client_connected:
			print(f'Client disconnected: {exception}')
			self.client.close()
			self.__is_client_connected = False
			
			if self.__keep_alive:
				self.__wait_client()

		if not self.__keep_alive:
			self.sock.close()

	def send_data(self, data: str) -> None:
		if not self.is_client_connected:
			return
		try:
			self._send_data(self.client, data)
		except Exception as e:
			self.close_connection(e)

	def __wait_client(self) -> None:
		try:
			self.sock.listen(1)
			print('Waiting for client...')
			self.client, address = self.sock.accept()
			print(f'Client connected: [{address[0]}:{address[1]}]')
			self.__is_client_connected = True
			self.__listen_client()
		except:
			pass

	def __listen_client(self) -> None:
		try:
			while self.__keep_alive:
				data = self._receive_data(self.client)

				for listener in self.__listeners:
					listener(data)
		except Exception as e:
			self.close_connection(e)

class Client(BaseSocket, Thread):
	@property
	def is_connected(self) -> bool:
		return self.__is_connected

	def __init__(self, ip: str, port: int):
		super().__init__()
		self.ip = ip
		self.port = port
		self.__is_connected = False
		self.__listeners: List[Callable[[str], None]] = []

	def run(self) -> None:
		try:
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			print(f'Connecting to {self.ip}:{self.port}...')
			self.sock.connect((self.ip, self.port))
			print(f'Connected to server [{self.ip}:{self.port}]')
			self.__is_connected = True
		except:
			print('Connection failed')
			self.__is_connected = False

		if self.is_connected:
			self.__listen_server()
		else:
			self.close_connection()

	def server_listener(self, handler: Callable[[str], None]) -> Callable[[str], None]:
		self.__listeners.append(handler)
		return handler

	def close_connection(self, exception: Exception | str='Connection closed') -> None:
		if self.is_connected:
			print(f'Connection lost: {exception}')
			self.sock.close()
			self.__is_connected = False

	def send_data(self, data: str) -> None:
		if not self.is_connected:
			return
		try:
			self._send_data(self.sock, data)
		except Exception as e:
			self.close_connection(e)

	def __listen_server(self) -> None:
		try:
			while self.is_connected:
				data = self._receive_data(self.sock)
				
				for listener in self.__listeners:
					listener(data)
		except Exception as e:
			self.close_connection(e)