#include "CPU.h"
#include "Tarefa.h"
#include <cstddef>

CPU::CPU(int id) {
    this->id = id;
    this->t = nullptr;
    this->ocupado = false;
    this->tempo_desligado = 0;
}

int CPU::getId() const { return id; }

Tarefa* CPU::getTarefa() const { return t; }

bool CPU::isOcupado() const { return ocupado; }

int CPU::getTempo_desligado() const { return tempo_desligado; }

void CPU::setTarefa(Tarefa* nova_tarefa) {
    this->t = nova_tarefa;
    this->ocupado = (nova_tarefa != nullptr);
}

void CPU::liberarCPU() {
    this->t = nullptr;
    this->ocupado = false;
}

void CPU::unidadeDesligado() {
    if (!ocupado) {
        tempo_desligado++;
    }
}