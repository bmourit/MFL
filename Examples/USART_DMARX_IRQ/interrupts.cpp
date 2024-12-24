#include "interrupts.hpp"
#include "EXAMPLE.hpp"

void DMA0_Channel3_IRQHandler(void) {
	if (example::EXAMPLE_DEVICE().dma_->get_interrupt_flag(dma::DMA_Channel::CHANNEL3, dma::Interrupt_Flags::INTR_FLAG_FTFIF) == true) {
		example::EXAMPLE_DEVICE().dma_->clear_interrupt_flag(dma::DMA_Channel::CHANNEL3, dma::Interrupt_Flags::INTR_FLAG_GIF);
		example::EXAMPLE_DEVICE().is_transfer_complete = true;
	}
}

void DMA0_Channel4_IRQHandler(void) {
	if (example::EXAMPLE_DEVICE().dma_->get_interrupt_flag(dma::DMA_Channel::CHANNEL4, dma::Interrupt_Flags::INTR_FLAG_FTFIF) == true) {
		example::EXAMPLE_DEVICE().dma_->clear_interrupt_flag(dma::DMA_Channel::CHANNEL4, dma::Interrupt_Flags::INTR_FLAG_GIF);
		example::EXAMPLE_DEVICE().is_transfer_complete = true;
	}
}
