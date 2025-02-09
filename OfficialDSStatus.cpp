#include <stdio.h>
#include "OfficialDSStatus.h"
#include "metamod_oslink.h"
#include "schemasystem/schemasystem.h"

OfficialDSStatus g_OfficialDSStatus;
PLUGIN_EXPOSE(OfficialDSStatus, g_OfficialDSStatus);

IVEngineServer2* engine = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;
CGlobalVars* gpGlobals = nullptr;

IUtilsApi* g_pUtils;

CGameEntitySystem* GameEntitySystem()
{
    return g_pUtils->GetCGameEntitySystem();
}

void StartupServer()
{
    g_pGameEntitySystem = GameEntitySystem();
    g_pEntitySystem = g_pUtils->GetCEntitySystem();
    gpGlobals = g_pUtils->GetCGlobalVars();
}

bool OfficialDSStatus::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);

    g_SMAPI->AddListener(this, this);

    return true;
}

bool OfficialDSStatus::Unload(char* error, size_t maxlen)
{
    ConVar_Unregister();
    return true;
}

void OfficialDSStatus::AllPluginsLoaded()
{
    char error[64];
    int ret;
    g_pUtils = (IUtilsApi*)g_SMAPI->MetaFactory(Utils_INTERFACE, &ret, NULL);
    if (ret == META_IFACE_FAILED)
    {
        g_SMAPI->Format(error, sizeof(error), "Missing Utils system plugin");
        ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
        std::string sBuffer = "meta unload " + std::to_string(g_PLID);
        engine->ServerCommand(sBuffer.c_str());
        return;
    }
    g_pUtils->StartupServer(g_PLID, StartupServer);
    g_pUtils->HookEvent(g_PLID, "round_start", [this](const char* szName, IGameEvent* pEvent, bool bDontBroadcast) {
        OnRoundStart();
    });
}

void OfficialDSStatus::OnRoundStart()
{
    CEntityInstance* pGameRulesProxy = UTIL_FindEntityByClassname("cs_gamerules");
    if (pGameRulesProxy)
    {
        CCSGameRules* pGameRules = ((CCSGameRulesProxy*)pGameRulesProxy)->m_pGameRules();
        if (pGameRules)
        {
            *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(pGameRules) + schema::GetServerOffset("CCSGameRules", "m_bIsValveDS")) = true;
        }
    }
}

const char* OfficialDSStatus::GetLicense()
{
    return "GPL";
}

const char* OfficialDSStatus::GetVersion()
{
    return "1.0";
}

const char* OfficialDSStatus::GetDate()
{
    return __DATE__;
}

const char* OfficialDSStatus::GetLogTag()
{
    return "OfficialDSStatus";
}

const char* OfficialDSStatus::GetAuthor()
{
    return "ABKAM";
}

const char* OfficialDSStatus::GetDescription()
{
    return "OfficialDSStatus";
}

const char* OfficialDSStatus::GetName()
{
    return "OfficialDSStatus";
}

const char* OfficialDSStatus::GetURL()
{
    return "";
}