#include "../include/interface.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>
#include "../include/execucao.hpp"

using namespace ProjetoSO;

Interface::Interface(unsigned int width, unsigned int height, const char *title)
	: screenWidth(width), screenHeight(height), title(title)
{
	InitWindow(screenWidth, screenHeight, title);
}

// desenha a grade de fundo do grafico de gantt
// além disso chama a função correspondente ao estado atual da interface
bool Interface::draw()
{
	BeginDrawing();
	inputHandler();

	switch (currentState)
	{
	case InterfaceState::Waiting:
	{
		drawWaiting();
		break;
	}
	case InterfaceState::Simulation:
	case InterfaceState::Paused:
	{
		if (drawSimulation())
			return true;
		break;
	}
	case InterfaceState::Results:
	{
		ClearBackground(RAYWHITE);
		DrawText("Simulação finalizada! Clique em voltar para reiniciar.", 10, 10, 20, DARKGRAY);
		unsigned int i = 0;
		for (CPU* cpu : Execucao::getInstance()->getCpuList())
		{
			DrawText(TextFormat("CPU %d: %d", cpu->getId(), cpu->getTempo_desligado()), 10 + 100 * i, 40, 20, DARKGRAY);
			i++;
		}
		drawBoard();
		drawGantt();
		if (drawButton(10, screenHeight - 40, "Voltar", BLUE))
			currentState = InterfaceState::Waiting;
		break;
	}
	}

	EndDrawing();
	return false;
}

// desenha a tela de espera, onde o usuário pode inserir as tarefas e iniciar a simulação
void Interface::drawWaiting()
{
	ClearBackground(RAYWHITE);
	DrawText("Aguardando entrada de tarefas...", 10, 10, 20, DARKGRAY);

	DrawText("Início", 10, 40, 10, DARKGRAY);
	drawTextfield(10, 50, inputStart, 1);
	DrawText("Duração", 100, 40, 10, DARKGRAY);
	drawTextfield(100, 50, inputDuration, 2);
	DrawText("Prioridade", 190, 40, 10, DARKGRAY);
	drawTextfield(190, 50, inputPriority, 3);
	if (drawButton(280, 50, "ADD", GREEN))
		addTask();

	drawBoard();
	drawTasks();

	int bottom = screenHeight - 40;
	if (drawButton(10, bottom, "Iniciar", BLUE))
	{
		initSimulation();
		EndDrawing();
		return;
	}
	DrawText("Escalonamento", 100, bottom - 10, 10, DARKGRAY);
	if (drawDropbox(100, bottom))
	{
		EndDrawing();
		return;
	}
	drawTextfield(190, bottom, inputQuantum, 4);
	DrawText("Quantum", 190, bottom - 10, 10, DARKGRAY);
	drawTextfield(280, bottom, inputCPUcount, 5);
	DrawText("CPUs", 280, bottom - 10, 10, DARKGRAY);
	if (drawButton(370, bottom, "Auto", YELLOW))
	{
		initSimulation();
		Execucao::getInstance()->automatico();
		currentState = InterfaceState::Results;
		EndDrawing();
		return;
	}
}

// desenha a simulação em andamento, onde o usuário pode avançar o relógio ou interagir com as tarefas em execução
bool Interface::drawSimulation()
{
	ClearBackground(RAYWHITE);
	unsigned int i = 0;
	for (CPU* cpu : Execucao::getInstance()->getCpuList())
	{
		DrawText(TextFormat("CPU %d", cpu->getId()), 10 + 40 * i, 40, 10, DARKGRAY);
		DrawRectangle(10 + 40 * i, 50, 30, 30, cpu->isOcupado() ? RED : GREEN);
		DrawRectangleLines(10 + 40 * i, 50, 30, 30, BLACK);
		i++;
	}

	if (currentState == InterfaceState::Paused)
		DrawText("Simulação pausada. Clique na ação que deseja realizar.", 10, 10, 20, DARKGRAY);
	else
		DrawText("Simulação em andamento, clique em próximo para avançar o relógio ou selecione uma tarefa.", 10, 10, 20, DARKGRAY);
	drawBoard();
	drawGantt();

	int bottom = screenHeight - 40;
	if (drawButton(10, bottom, "Cancel", RED))
		currentState = InterfaceState::Waiting;
	if (drawButton(100, bottom, "Prox", GREEN))
	{
		currentState = InterfaceState::Simulation;
		EndDrawing();
		return true;
	}

	if (currentState == InterfaceState::Paused)
	{
		if (selectedTask == nullptr)
			currentState = InterfaceState::Simulation;
		else
		{
			if (drawButton(190, bottom, "Remover", RED, 120)) 
			{
				Execucao::getInstance()->finalizar_tarefa(selectedTask);
				selectedTask = nullptr;
			}
			if (drawButton(320, bottom, "Pronto", YELLOW)) 
			{
				Execucao::getInstance()->mandar_fila_prontos(selectedTask);
				selectedTask = nullptr;
			}
			if (drawButton(410, bottom, "CPU", GREEN)) 
			{
				Execucao::getInstance()->mandar_cpu(selectedTask);
				selectedTask = nullptr;
			}
		}
	}
	return false;
}

// desenha as tarefas no gráfico de gantt, considerando o tempo atual do relógio
void Interface::drawGantt()
{
	unsigned int clock = Execucao::getInstance()->getRelogio();
	std::vector<Tarefa*> tarefas = Execucao::getInstance()->getTarefas();
	for (const CPU* cpu : Execucao::getInstance()->getCpuList())
	{
		Tarefa* t = cpu->getTarefa();
		if (t != nullptr)
			tarefas.push_back(t);
	}

	for (Tarefa* t : Execucao::getInstance()->getFinalizadas())
		tarefas.push_back(t);

	for (const Tarefa* t : tarefas)
	{
		std::vector<Tarefa::Event> events = t->getEvents();
		for (size_t i = 0; i < events.size(); i++)
		{
			Tarefa::Event ev = events[i];
			if (ev.state == Tarefa::TaskState::Finished) break;
			if (events.size() > i + 2 && events[i + 1].begin < clock)
			{
				ev.end = events[i + 1].begin;
				if (ev.begin == ev.end)
				{
					events.erase(events.begin() + i);
					i--;
					continue;
				}
			}
			unsigned int x = 20 + 30 * ev.begin;
			unsigned int y = (screenHeight - 100) - 30 * (t->getId() - 1);
			unsigned int width = ev.begin == ev.end ? clock - ev.begin : ev.end - ev.begin;
			std::cout << "clock: " << clock << ", tarefa: " << t->getId() << ", estado: " << static_cast<int>(ev.state) << ", início: " << ev.begin << ", fim: " << ev.end << std::endl;
			Color color = WHITE;
			if (ev.state == Tarefa::TaskState::Executing) color = t->getColor();
			if (drawTask(t->getPrioridade(), x, y, width, color))
			{
				selectedTask = const_cast<Tarefa*>(t);
				currentState = InterfaceState::Paused;
				return;
			}
		}
	}
}

// desenha um campo de texto, onde o usuário pode clicar para selecionar e digitar um valor, o id é usado para identificar qual campo está selecionado
void Interface::drawTextfield(unsigned int x, unsigned int y, char *input, unsigned int id)
{
	Vector2 mouse = GetMousePosition();
	bool hovered = mouse.x > x && mouse.x < x + 80 && mouse.y > y && mouse.y < y + 30;
	if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		currentTarget = id;

	DrawRectangle(x, y, 80, 30, LIGHTGRAY);
	if (currentTarget == id)
		DrawRectangleLines(x, y, 80, 30, BLUE);
	else
		DrawRectangleLines(x, y, 80, 30, BLACK);
	DrawText(input, x + 5, y + 5, 20, BLACK);
}


// desenha um botão, onde o usuário pode clicar para realizar uma ação, retorna true se o botão foi clicado
bool Interface::drawButton(unsigned int x, unsigned int y, const char *text, Color color, unsigned int width)
{
	Vector2 mouse = GetMousePosition();
	bool hovered = mouse.x > x && mouse.x < x + width && mouse.y > y && mouse.y < y + 30;
	bool wasClicked = false;
	if (hovered)
	{
		color = ColorBrightness(color, 0.2f);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			wasClicked = true;
	}

	DrawRectangle(x, y, width, 30, color);
	DrawRectangleLines(x, y, width, 30, BLACK);
	int textWidth = MeasureText(text, 20);
	unsigned int colorValue = (color.r << 16) | (color.g << 8) | color.b;
	DrawText(text, x + (width - textWidth) / 2, y + 5, 20, colorValue > 0xAAAAAA ? BLACK : WHITE);

	return wasClicked;
}

// desenha uma caixa de seleção, onde o usuário pode clicar para selecionar o tipo de escalonamento, retorna true se o tipo foi alterado
bool Interface::drawDropbox(unsigned int x, unsigned int y)
{
	Vector2 mouse = GetMousePosition();
	bool hovered = mouse.x > x && mouse.x < x + 80 && mouse.y > y && mouse.y < y + 30;
	if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		showingDropbox = !showingDropbox;

	DrawRectangle(x, y, 80, 30, LIGHTGRAY);
	DrawRectangleLines(x, y, 80, 30, BLACK);
	DrawText(getSimTypeText(simulationType).c_str(), x + 5, y + 5, 20, BLACK);

	if (showingDropbox)
	{
		for (int i = 0; i < opcount; i++)
		{
			Rectangle optionRect = {(float)x, (float)y - 30 * (i + 1), 80, 30};

			if (CheckCollisionPointRec(mouse, optionRect))
			{
				DrawRectangleRec(optionRect, SKYBLUE);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					simulationType = static_cast<SimulationType>(i);
					showingDropbox = false;
					return true;
				}
			}
			else
				DrawRectangleRec(optionRect, LIGHTGRAY);
			DrawRectangleLinesEx(optionRect, 1, GRAY);
			DrawText(getSimTypeText(static_cast<SimulationType>(i)).c_str(), optionRect.x + 10, optionRect.y + optionRect.height / 2 - 10, 20, BLACK);
		}
	}
	return false;
}

// desenha as tarefas, utilizado na tela de espera para mostrar quais tarefas vão ser executadas
void Interface::drawTasks()
{
	bool removed = false;
	for (size_t i = 0; i < tasks.size(); i++)
	{
		unsigned int x = 20 + 30 * tasks[i]->getTempoIngresso();
		unsigned int y = (screenHeight - 100) - 30 * i;
		if (drawTask(tasks[i]->getPrioridade(), x, y, tasks[i]->getTempoTotal(), tasks[i]->getColor()) && !removed)
		{
			delete tasks[i];
			tasks.erase(tasks.begin() + i);
			i--;
			removed = true;
		}
	}
}

// desenha uma tarefa na tela, escreve prioridade dentro do quadrado da tarefa
// retorna true se a tarefa foi clicada
bool Interface::drawTask(unsigned int priority, unsigned int x, unsigned int y, unsigned int taskWidth, Color color)
{
	Vector2 mouse = GetMousePosition();
	unsigned int width = taskWidth * 30;

	bool hovered = mouse.x > x && mouse.x < x + width && mouse.y > y && mouse.y < y + 30;
	bool wasClicked = false;
	if (hovered && !showingDropbox) // TODO: MELHORAR ISSO
	{
		color = ColorBrightness(color, 0.2f);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			wasClicked = true;
	}

	DrawRectangle(x, y, width, 30, color);
	DrawRectangleLines(x, y, width, 30, BLACK);

	unsigned int colorValue = (color.r << 16) | (color.g << 8) | color.b;
	DrawText(TextFormat("%d", priority), x + 5, y + 5, 20, colorValue > 0xAAAAAA ? BLACK : WHITE);

	return wasClicked;
}

// desenha a grade na tela, local onde as tarefas são mostradas
void Interface::drawBoard()
{
	DrawRectangle(10, 90, screenWidth - 20, screenHeight - 145, Color({225, 225, 225, 255}));
	DrawLine(20, 90, 20, screenHeight - 70, GRAY);
	DrawLine(20, screenHeight - 70, screenWidth - 10, screenHeight - 70, GRAY);
	DrawText("0", 20, screenHeight - 65, 10, BLACK);
	unsigned int count = 1;
	for (int i = 50; i < screenWidth; i += 30)
	{
		for (int j = 90; j < screenHeight - 70; j += 15)
			DrawLine(i, j, i, j + 5, GRAY);
		DrawText(TextFormat("%d", count++), i, screenHeight - 65, 10, BLACK);
	}
}

// trata entrada do teclado, utilizado para os campos de texto de acordo com o campo selecionado no momento
void Interface::inputHandler()
{
	if (currentTarget != 0)
	{
		char *buffer;
		switch (currentTarget)
		{
		case 1: buffer = inputStart; 	break;
		case 2:	buffer = inputDuration;	break;
		case 3:	buffer = inputPriority;	break;
		case 4:	buffer = inputQuantum; 	break;
		case 5:	buffer = inputCPUcount;	break;
		default: return;
		}

		int key = GetKeyPressed();

		while (key > 0)
		{
			if (key == KEY_BACKSPACE && strlen(buffer) > 0)
				buffer[strlen(buffer) - 1] = '\0';
			else if (key >= '0' && key <= '9')
			{
				if (strlen(buffer) < 8)
				{
					char c = (char)key;
					buffer[strlen(buffer)] = c;
					buffer[strlen(buffer) + 1] = '\0';
				}
			}
			key = GetKeyPressed();
		}
	}
}

// adiciona uma nova tarefa a lista de tarefas, utilizado para mostrar na tela e para execução
void Interface::addTask()
{
	if (strlen(inputStart) == 0 || strlen(inputDuration) == 0 || strlen(inputPriority) == 0)
		return;

	int start = atoi(inputStart);
	int duration = atoi(inputDuration);
	int priority = atoi(inputPriority);
	Color color = {(unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255};
	Tarefa *newTask = new Tarefa(tasks.size() + 1, color, start, duration, priority);
	tasks.push_back(newTask);
	inputStart[0] = '\0';
	inputDuration[0] = '\0';
	inputPriority[0] = '\0';

	std::sort(tasks.begin(), tasks.end(), [](Tarefa *a, Tarefa *b)
			  { return a->getTempoIngresso() < b->getTempoIngresso(); });
}

// inicia a simulação, passando parâmetros para a execução
void Interface::initSimulation()
{
	ClearBackground(RAYWHITE);
	std::sort(tasks.begin(), tasks.end(), [](Tarefa *a, Tarefa *b)
			  { return a->getTempoIngresso() < b->getTempoIngresso(); });
	std::cout << ">> Iniciando simulação com " << tasks.size() << " tarefas." << std::endl;
	Execucao::getInstance()->init(getSimTypeText(simulationType), tasks, atoi(inputQuantum), atoi(inputCPUcount));
	currentState = InterfaceState::Simulation;
	Execucao::getInstance()->update();
}

// retorna em forma de texto o tipo de escalonamento
std::string Interface::getSimTypeText(SimulationType type) const
{
	switch (type)
	{
	case SimulationType::SRTF:
		return "SRTF";
	case SimulationType::PRIOP:
		return "PRIOP";
	case SimulationType::PRIOPENV:
		return "PRIOPENV";
	case SimulationType::FCFS:
		return "FCFS";
	case SimulationType::RR:
		return "RR";
	case SimulationType::PRIOD:
		return "PRIOD";
	case SimulationType::SJF:
		return "SJF";
	default:
		return "Desconecido";
	}
}

// seta o tipo de escalonamento, tendo como parâmetro uma string, para facilitar leitura de arquivo
void Interface::setSimulationType(std::string type)
{
	std::transform(type.begin(), type.end(), type.begin(), ::toupper);
	if (type == "SRTF")
	{
		simulationType = SimulationType::SRTF;
	}
	else if (type == "PRIOP")
		simulationType = SimulationType::PRIOP;
	else if (type == "PRIOPENV")
		simulationType = SimulationType::PRIOPENV;
	else if (type == "FCFS")
		simulationType = SimulationType::FCFS;
	else if (type == "RR")
		simulationType = SimulationType::RR;
	else if (type == "PRIOD")
		simulationType = SimulationType::PRIOD;
	else if (type == "SJF")
		simulationType = SimulationType::SJF;
}

void Interface::simulationFinished()
{
	currentState = InterfaceState::Results;
}