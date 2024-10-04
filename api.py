import uvicorn
from fastapi import FastAPI
import datetime
import time
from loguru import logger
from pydantic import BaseModel
# IMPORTANT: to reproduce, need these from the ambolt repo:
from sim.dtos import TrafficSimulationPredictResponseDto, TrafficSimulationPredictRequestDto, SignalDto

HOST = "localhost"
PORT = 9052

app = FastAPI()
start_time = time.time()

@app.get('/api')
def hello():
	return {
		"service": "traffic-simulation-usecase",
		"uptime": '{}'.format(datetime.timedelta(seconds=time.time() - start_time))
	}

@app.get('/')
def index():
	return "Your endpoint is running!"

from model import Model

model = Model()

@app.post('/predict', response_model=TrafficSimulationPredictResponseDto)
def predict_endpoint(request: TrafficSimulationPredictRequestDto):
	data = request
	vehicles = data.vehicles
	total_score = data.total_score
	simulation_ticks = data.simulation_ticks
	signals = data.signals
	signal_groups = data.signal_groups
	legs = data.legs
	allowed_green_signal_combinations = data.allowed_green_signal_combinations
	is_terminated = data.is_terminated

	prediction = {}
	prediction["signals"] = []

	try:
		global model
		result = model.update_prediction(request, prediction)
	except Exception as e:
		# TODO: TEST THIS, DO A THROW, TEST RECOVERY
		print(e)
		logger.error(e)
		model.destroy()
		model = Model()
		return TrafficSimulationPredictResponseDto(signals=[])

	response = TrafficSimulationPredictResponseDto(signals=prediction["signals"])

	return response

if __name__ == '__main__':
	uvicorn.run('api:app', host=HOST, port=PORT)
