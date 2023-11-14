#include "UpdateManager.h"

SINGLETONBODY(DeviousDevices::UpdateManager)

void DeviousDevices::UpdateManager::Setup()
{
    if (!_installed)
    {
        _installed = true;
        REL::Relocation<std::uintptr_t> vtbl{RE::PlayerCharacter::VTABLE[0]};
        Update_old = vtbl.write_vfunc(0x0AD, Update);
    }
}

//this function is only called if no menu is open. It also looks like that it is not called when player is in free cam mode
void DeviousDevices::UpdateManager::Update(RE::Actor* a_actor, float a_delta)
{
    static RE::Actor* loc_player = RE::PlayerCharacter::GetSingleton();
    UpdateManager* loc_manager = UpdateManager::GetSingleton();
    if (a_actor == loc_player)
    {
        if (!loc_manager->UpdateThread500) std::thread([loc_manager]
        {
            loc_manager->UpdateThread500 = true;

            //serialize task so it doesnt create race condition
            SKSE::GetTaskInterface()->AddTask([]
            {
                //update node hider
                NodeHider::GetSingleton()->Update();

                //update gag expressions
                ExpressionManager::GetSingleton()->UpdateGagExpForNPCs();
            });

            //wait
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); //wait 500ms before updating again
            loc_manager->UpdateThread500 = false;
        }).detach();

    }
    Update_old(a_actor,a_delta);
}