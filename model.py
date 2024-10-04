import warnings
from loguru import logger
import subprocess
import time
import sys

class Model:
	def __init__(self):
		self.is_initialized = False

		self.executable = "./exe2c"

		self.process = None

		self.prev_score = 1e9

	def destroy(self):
		if self.process is not None:
			input_data = "$q\n"
			self.process.stdin.write(input_data)
			self.process.stdin.flush()
			time.sleep(0.3)
			self.process.terminate()

	def update_prediction(self, state, prediction):
		if state.simulation_ticks <= 10:
			return { }

		self.log_info(state)

		if self.prev_score > state.total_score:
			if self.process is not None:
				try:
					input_data = "$q\n"
					self.process.stdin.write(input_data)
					self.process.stdin.flush()
					time.sleep(0.1)
					self.process.terminate()
					self.process.wait(timeout = 0.2)
					if self.process.poll() is None:
						self.process.kill()
						self.process.wait(timeout = 0.1)
				except Exception as e:
					self.LOG(f"error during subprocess resetting: {e}")
				self.process = None
			self.process = subprocess.Popen([self.executable], stdin = subprocess.PIPE, stdout = subprocess.PIPE, text = True)
			time.sleep(0.1)

		self.prev_score = state.total_score

		input_data = ""

		input_data += f"{len(state.vehicles)}\n"
		for vehicle in state.vehicles:
			input_data += f"{vehicle.speed:.9f} {vehicle.distance_to_stop:.9f} {vehicle.leg}\n"
		input_data += f"{state.total_score:.9f}\n"
		input_data += f"{state.simulation_ticks}\n"
		input_data += f"{len(state.signals)}\n"
		for signal in state.signals:
			input_data += f"{signal.name} {signal.state}\n"
		input_data += f"{len(state.signal_groups)}\n"
		for signal_group in state.signal_groups:
			input_data += f"{signal_group}\n"
		input_data += f"{len(state.legs)}\n"
		for leg in state.legs:
			input_data += f"{leg.name} {len(leg.lanes)} {len(leg.signal_groups)}\n"
			for lane in leg.lanes:
				input_data += f"{lane}\n"
			for signal_group in leg.signal_groups:
				input_data += f"{signal_group}\n"
		input_data += f"{len(state.allowed_green_signal_combinations)}\n"
		for allowed_green_signal_combination in state.allowed_green_signal_combinations:
			input_data += f"{allowed_green_signal_combination.name} {len(allowed_green_signal_combination.groups)}\n"
			for group in allowed_green_signal_combination.groups:
				input_data += f"{group}\n"

		lines = self.send_process_cycle(input_data)

		for line in lines:
			if line and len(line.split(" ")) == 2:
				signal_name, signal_state = line.split(" ")
				self.update_pred(prediction, signal_name, signal_state)
			elif not line:
				self.LOG("error, signal update command was None; ignoring signal update attempt")
			else:
				self.LOG(f"error, expected two elements from: '{line}'.split(); ignoring signal update attempt")

	def update_pred(self, prediction, signal_name, signal_state):
		prediction['signals'].append({'name': signal_name, 'state': signal_state})

	def send_process_cycle(self, input_data):
		if self.process is None:
			return None
		try:
			self.process.stdin.write("$o\n")
			self.process.stdin.write(input_data)
			self.process.stdin.write("$oo\n")
			self.process.stdin.flush()
			output = []
			while True:
				line = self.process.stdout.readline().strip().strip("\n")
				if line == "$o":
					continue
				if line == "$oo":
					break
				if line and line != "":
					output.append(line)
			return output
		except Exception as e:
			return None

	def LOG(self, msg):
		logger.info(f"{msg}")
		with open("log-thor.txt", "a") as f:
			f.write(f"{msg}\n")

	def log_info(self, state):
		self.LOG(f"[tick: {state.simulation_ticks}, score: {state.total_score}, vehicles: {len(state.vehicles)}")
		pass
