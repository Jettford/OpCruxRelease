#include "Entity/Components/BuildBorderComponent.hpp"
#include "Entity/GameObject.hpp"
#include "Entity/GameMessages/RequestUse.hpp"
#include "Entity/Components/InventoryComponent.hpp"

void BuildBorderComponent::OnRequestUse(Entity::GameObject* sender, GM::RequestUse& msg) {
	// Get inventory

	GM::StartArrangingWithItem myMsg;
	myMsg.buildAreaID = owner->GetObjectID();
	myMsg.buildStartPOS = sender->GetPosition();
	myMsg.sourceTYPE = 8;

	GameMessages::Broadcast(sender, myMsg);
}

void BuildBorderComponent::OnSetBuildMode(Entity::GameObject* sender, GM::SetBuildMode& msg) {
	auto invComp = sender->GetComponent<InventoryComponent>();

	if (msg.bStart) {
		// auto equip hat
		if (!invComp->hasEquipped(thinkingHatLOT)) {
			// We do no have it equipped, auto equip
			invComp->EquipItem(thinkingHatLOT);
		}
	}
	else {
		// auto unequip hat
		if (invComp->hasEquipped(thinkingHatLOT)) {
			// We do no have it equipped, auto equip
			invComp->UnEquipItem(thinkingHatLOT);
		}
	}
}

void BuildBorderComponent::OnStartBuildingWithItem(Entity::GameObject* sender, GM::StartBuildingWithItem& msg) {
	// Check if thinking hat, if not return
	if (msg.sourceLOT != thinkingHatLOT) return;

	// Get inventory
	auto invComp = sender->GetComponent<InventoryComponent>();

	// auto equip hat
	if (!invComp->hasEquipped(thinkingHatLOT)) {
		// We do no have it equipped, auto equip
		invComp->EquipItem(thinkingHatLOT);
	}

	GM::StartArrangingWithItem myMsg;
	myMsg.buildAreaID = owner->GetObjectID();
	myMsg.buildStartPOS = sender->GetPosition();
	myMsg.sourceTYPE = 8;

	GameMessages::Broadcast(sender, myMsg);
}