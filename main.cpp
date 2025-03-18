#include "main.h"

#define PUSH_ALL __asm {\
    __asm push eax          \
    __asm push ebx          \
    __asm push ecx          \
    __asm push edx          \
    __asm push esi          \
    __asm push edi          \
    __asm push ebp          \
    __asm push esp          \
}
#define POP_ALL __asm {\
    __asm pop esp          \
    __asm pop ebp          \
    __asm pop edi          \
    __asm pop esi          \
    __asm pop edx          \
    __asm pop ecx          \
    __asm pop ebx          \
    __asm pop eax          \
}

UINT_PTR base;
unsigned int DisplaNameInjection_JMP_back;

class Creature {
public:
    unsigned int field_0;
    unsigned int field_4;
    long long int GUID;
    long long unsigned int x, y, z; //0x10 ~ 0x27
    char padding2[0x60];
    float physical_size; //0x88
    char padding1[0xD4];
    float camera_offsetx;
    float camera_offsey;
    float camera_offsetz;
    float HP;
    float MP;
    float block_power;
    float HP_multiplier;
    float attack_speed_multiplier;
    float damage_multiplier;
    float armor_multiplier;
    float resistance_multiplier;
    char field_18C;
    char field_18D;
    char field_18E;
    char field_18F;
    int level;
    int XP;
    long long parent_GUID;

};

static std::wstring* __stdcall DisplayNameOverwrite(std::wstring* string, Creature* creature) { // Creature* creature
    //wchar_t buffer[250];
    //swprintf_s(buffer, 250, L"LVL %d ", creature->level);
    std::wstring mob_str = std::wstring(string->c_str());
    std::wstring* final_str = new std::wstring(L"LV " + std::to_wstring(creature->level) + L" " + mob_str);
    return final_str;
}

void __declspec(naked) DisplaNameInjection() {
    //original code
    __asm {

        PUSH_ALL

        //custom code
        mov eax, [esp + 0x24] // 0x8
        mov ebx, [edi + 0x8006D0]
        push ebx
        push eax

        call DisplayNameOverwrite

        mov [esp + 0x24], eax // 0x8
        //pop eax
        //pop ebx

        POP_ALL

        //original code
        mov byte ptr[esp + 0x00004770], -0x1B
        jmp[DisplaNameInjection_JMP_back];
    }
}

void WriteJMP(BYTE* location, BYTE* newFunction) {
    DWORD dwOldProtection;
    VirtualProtect(location, 8, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(((unsigned int)newFunction - (unsigned int)location) - 5);
    location[5] = 0x90; //nop
    location[6] = 0x90; //nop
    location[7] = 0x90; //nop
    VirtualProtect(location, 8, dwOldProtection, &dwOldProtection);
}

extern "C" BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) // DLL_EXPORT
{
    base = (UINT_PTR)GetModuleHandle(NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DisplaNameInjection_JMP_back = base + 0x9AE3D;
            WriteJMP((BYTE*)(base + 0x9AE36), (BYTE*)&DisplaNameInjection);
            break;
    }
    return TRUE;
}
