#pragma once
#include <string>
#include "raylib.h"
#include <vector>
#include "tarefa.hpp"

namespace ProjetoSO
{
	class Interface
	{
	public:
		enum class InterfaceState
		{
			Waiting,
			Simulation,
			Paused,
			Results
		};
	protected:
		

		enum class SimulationType
		{
			SRTF,
			PRIOP
		};


		const unsigned int screenWidth;
		const unsigned int screenHeight;
		const char *title;

		char inputStart[9] = "";
		char inputDuration[9] = "";
		char inputPriority[9] = "";
		char inputQuantum[9] = "";
		char inputCPUcount[9] = "";
		char currentTarget = 0; // 0: nenhum, 1: start, 2: duration, 3: priority, 4: quantum, 5: cpu count

		SimulationType simulationType = SimulationType::SRTF;
		bool showingDropbox = false;
		const int opcount = 2;

		InterfaceState currentState = InterfaceState::Waiting;
		std::vector<Tarefa *> tasks;

		Tarefa *selectedTask = nullptr;
		
		void drawTextfield(unsigned int x, unsigned int y, char *input, unsigned int id);
		bool drawButton(unsigned int x, unsigned int y, const char *text, Color color, unsigned int width = 80);
		bool drawDropbox(unsigned int x, unsigned int y);
		void drawTasks();
		bool drawTask(unsigned int priority, unsigned int x, unsigned int y, unsigned int taskWidth, Color color);
		void drawBoard();
		void drawGantt();
		void drawWaiting();
		bool drawSimulation();
		void inputHandler();
		void addTask();
		void initSimulation();

	public:
		Interface(unsigned int width, unsigned int height, const char *title);
		virtual ~Interface() = default;
		bool draw();
		void simulationFinished();

		std::string getSimTypeText(SimulationType type) const;

		void setTasks(const std::vector<Tarefa *> &newTasks) { tasks = newTasks; }
		void setQuantum(unsigned int q) { snprintf(inputQuantum, sizeof(inputQuantum), "%d", q); }
		void setCPUCount(unsigned int c) { snprintf(inputCPUcount, sizeof(inputCPUcount), "%d", c); }
		void setSimulationType(std::string type);
	};
}