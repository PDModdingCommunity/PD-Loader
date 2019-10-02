#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#pragma once
#include <string>
#include <shlobj.h>
#include <set>
#include "ModuleList.h"
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

struct shared
{
	FARPROC DllCanUnloadNow;
	FARPROC DllGetClassObject;
	FARPROC DllRegisterServer;
	FARPROC DllUnregisterServer;
	FARPROC DebugSetMute;

	void LoadOriginalLibrary(HMODULE dll)
	{
		DllCanUnloadNow = GetProcAddress(dll, "DllCanUnloadNow");
		DllGetClassObject = GetProcAddress(dll, "DllGetClassObject");
		DllRegisterServer = GetProcAddress(dll, "DllRegisterServer");
		DllUnregisterServer = GetProcAddress(dll, "DllUnregisterServer");
		DebugSetMute = GetProcAddress(dll, "DebugSetMute");
	}
} shared;

struct dnsapi_dll
{
	HMODULE dll;

	// generated using windows 7 dnsapi.dll
	// functions not in vista are commented out manually
	// needed windows 8+ have been manually added so they can be stubbed
	FARPROC BreakRecordsIntoBlob;
	FARPROC CombineRecordsInBlob;
	FARPROC DnsAcquireContextHandle_A;
	FARPROC DnsAcquireContextHandle_W;
	FARPROC DnsAllocateRecord;
	FARPROC DnsApiAlloc;
	FARPROC DnsApiAllocZero;
	FARPROC DnsApiFree;
	FARPROC DnsApiHeapReset;
	FARPROC DnsApiRealloc;
	FARPROC DnsApiSetDebugGlobals;
	FARPROC DnsAsyncRegisterHostAddrs;
	FARPROC DnsAsyncRegisterInit;
	FARPROC DnsAsyncRegisterTerm;
	FARPROC DnsCancelQuery;
	FARPROC DnsCopyStringEx;
	FARPROC DnsCreateReverseNameStringForIpAddress;
	FARPROC DnsCreateStandardDnsNameCopy;
	FARPROC DnsCreateStringCopy;
	FARPROC DnsDhcpRegisterAddrs;
	FARPROC DnsDhcpRegisterHostAddrs;
	FARPROC DnsDhcpRegisterInit;
	FARPROC DnsDhcpRegisterTerm;
	FARPROC DnsDhcpRemoveRegistrations;
	FARPROC DnsDhcpSrvRegisterHostAddr;
	//FARPROC DnsDhcpSrvRegisterHostAddrEx;
	FARPROC DnsDhcpSrvRegisterHostName;
	//FARPROC DnsDhcpSrvRegisterHostNameEx;
	FARPROC DnsDhcpSrvRegisterInit;
	FARPROC DnsDhcpSrvRegisterInitialize;
	FARPROC DnsDhcpSrvRegisterTerm;
	FARPROC DnsDowncaseDnsNameLabel;
	FARPROC DnsExtractRecordsFromMessage_UTF8;
	FARPROC DnsExtractRecordsFromMessage_W;
	FARPROC DnsFindAuthoritativeZone;
	FARPROC DnsFlushResolverCache;
	FARPROC DnsFlushResolverCacheEntry_A;
	FARPROC DnsFlushResolverCacheEntry_UTF8;
	FARPROC DnsFlushResolverCacheEntry_W;
	FARPROC DnsFree;
	FARPROC DnsFreeConfigStructure;
	//FARPROC DnsFreePolicyConfig;
	//FARPROC DnsFreeProxyName;
	FARPROC DnsGetBufferLengthForStringCopy;
	FARPROC DnsGetCacheDataTable;
	FARPROC DnsGetDnsServerList;
	FARPROC DnsGetDomainName;
	FARPROC DnsGetLastFailedUpdateInfo;
	//FARPROC DnsGetPolicyTableInfo;
	//FARPROC DnsGetPolicyTableInfoPrivate;
	FARPROC DnsGetPrimaryDomainName_A;
	//FARPROC DnsGetProxyInfoPrivate;
	//FARPROC DnsGetProxyInformation;
	FARPROC DnsGlobals;
	FARPROC DnsIpv6AddressToString;
	FARPROC DnsIpv6StringToAddress;
	FARPROC DnsIsAMailboxType;
	FARPROC DnsIsStatusRcode;
	FARPROC DnsIsStringCountValidForTextType;
	FARPROC DnsLogEvent;
	FARPROC DnsLogIn;
	FARPROC DnsLogInit;
	FARPROC DnsLogIt;
	FARPROC DnsLogOut;
	FARPROC DnsLogTime;
	FARPROC DnsMapRcodeToStatus;
	FARPROC DnsModifyRecordsInSet_A;
	FARPROC DnsModifyRecordsInSet_UTF8;
	FARPROC DnsModifyRecordsInSet_W;
	FARPROC DnsNameCompareEx_A;
	FARPROC DnsNameCompareEx_UTF8;
	FARPROC DnsNameCompareEx_W;
	FARPROC DnsNameCompare_A;
	FARPROC DnsNameCompare_UTF8;
	FARPROC DnsNameCompare_W;
	FARPROC DnsNameCopy;
	FARPROC DnsNameCopyAllocate;
	FARPROC DnsNetworkInfo_CreateFromFAZ;
	FARPROC DnsNetworkInformation_CreateFromFAZ;
	FARPROC DnsNotifyResolver;
	FARPROC DnsNotifyResolverClusterIp;
	FARPROC DnsNotifyResolverEx;
	FARPROC DnsQueryConfig;
	FARPROC DnsQueryConfigAllocEx;
	FARPROC DnsQueryConfigDword;
	FARPROC DnsQueryEx;
	FARPROC DnsQueryExA;
	FARPROC DnsQueryExUTF8;
	FARPROC DnsQueryExW;
	FARPROC DnsQuery_A;
	FARPROC DnsQuery_UTF8;
	FARPROC DnsQuery_W;
	FARPROC DnsRecordBuild_UTF8;
	FARPROC DnsRecordBuild_W;
	FARPROC DnsRecordCompare;
	FARPROC DnsRecordCopyEx;
	FARPROC DnsRecordListFree;
	FARPROC DnsRecordSetCompare;
	FARPROC DnsRecordSetCopyEx;
	FARPROC DnsRecordSetDetach;
	FARPROC DnsRecordStringForType;
	FARPROC DnsRecordStringForWritableType;
	FARPROC DnsRecordTypeForName;
	FARPROC DnsRegisterClusterAddress;
	FARPROC DnsReleaseContextHandle;
	FARPROC DnsRemoveRegistrations;
	FARPROC DnsReplaceRecordSetA;
	FARPROC DnsReplaceRecordSetUTF8;
	FARPROC DnsReplaceRecordSetW;
	FARPROC DnsResolverOp;
	FARPROC DnsScreenLocalAddrsForRegistration;
	FARPROC DnsSetConfigDword;
	FARPROC DnsStatusString;
	FARPROC DnsStringCopyAllocateEx;
	//FARPROC DnsTraceServerConfig;
	FARPROC DnsUnicodeToUtf8;
	FARPROC DnsUpdate;
	//FARPROC DnsUpdateMachinePresence;
	FARPROC DnsUpdateTest_A;
	FARPROC DnsUpdateTest_UTF8;
	FARPROC DnsUpdateTest_W;
	FARPROC DnsUtf8ToUnicode;
	FARPROC DnsValidateNameOrIp_TempW;
	FARPROC DnsValidateName_A;
	FARPROC DnsValidateName_UTF8;
	FARPROC DnsValidateName_W;
	//FARPROC DnsValidateServerArray_A; // available in vista sp2
	//FARPROC DnsValidateServerArray_W; // available in vista sp2
	//FARPROC DnsValidateServerStatus;
	//FARPROC DnsValidateServer_A; // available in vista sp2
	//FARPROC DnsValidateServer_W; // available in vista sp2
	FARPROC DnsValidateUtf8Byte;
	FARPROC DnsWriteQuestionToBuffer_UTF8;
	FARPROC DnsWriteQuestionToBuffer_W;
	FARPROC DnsWriteReverseNameStringForIpAddress;
	FARPROC Dns_AddRecordsToMessage;
	FARPROC Dns_AllocateMsgBuf;
	FARPROC Dns_BuildPacket;
	FARPROC Dns_CleanupWinsock;
	FARPROC Dns_CloseConnection;
	FARPROC Dns_CloseSocket;
	FARPROC Dns_CreateMulticastSocket;
	FARPROC Dns_CreateSocket;
	FARPROC Dns_CreateSocketEx;
	FARPROC Dns_ExtractRecordsFromMessage;
	FARPROC Dns_FindAuthoritativeZoneLib;
	FARPROC Dns_FreeMsgBuf;
	FARPROC Dns_GetRandomXid;
	FARPROC Dns_InitializeMsgBuf;
	FARPROC Dns_InitializeMsgRemoteSockaddr;
	FARPROC Dns_InitializeWinsock;
	FARPROC Dns_OpenTcpConnectionAndSend;
	FARPROC Dns_ParseMessage;
	FARPROC Dns_ParsePacketRecord;
	FARPROC Dns_PingAdapterServers;
	FARPROC Dns_ReadPacketName;
	FARPROC Dns_ReadPacketNameAllocate;
	FARPROC Dns_ReadRecordStructureFromPacket;
	FARPROC Dns_RecvTcp;
	FARPROC Dns_ResetNetworkInfo;
	FARPROC Dns_SendAndRecvUdp;
	FARPROC Dns_SendEx;
	FARPROC Dns_SetRecordDatalength;
	FARPROC Dns_SetRecordsSection;
	FARPROC Dns_SetRecordsTtl;
	FARPROC Dns_SkipPacketName;
	FARPROC Dns_SkipToRecord;
	FARPROC Dns_UpdateLib;
	FARPROC Dns_UpdateLibEx;
	FARPROC Dns_WriteDottedNameToPacket;
	FARPROC Dns_WriteQuestionToMessage;
	FARPROC Dns_WriteRecordStructureToPacketEx;
	FARPROC ExtraInfo_Init;
	FARPROC Faz_AreServerListsInSameNameSpace;
	//FARPROC FlushDnsPolicyUnreachableStatus;
	FARPROC GetCurrentTimeInSeconds;
	FARPROC HostsFile_Close;
	FARPROC HostsFile_Open;
	FARPROC HostsFile_ReadLine;
	FARPROC IpHelp_IsAddrOnLink;
	FARPROC Local_GetRecordsForLocalName;
	FARPROC Local_GetRecordsForLocalNameEx;
	FARPROC NetInfo_Build;
	FARPROC NetInfo_Clean;
	FARPROC NetInfo_Copy;
	FARPROC NetInfo_Free;
	FARPROC NetInfo_GetAdapterByAddress;
	FARPROC NetInfo_GetAdapterByInterfaceIndex;
	FARPROC NetInfo_GetAdapterByName;
	FARPROC NetInfo_IsAddrConfig;
	FARPROC NetInfo_IsForUpdate;
	FARPROC NetInfo_ResetServerPriorities;
	//FARPROC NetInfo_UpdateServerReachability;
	FARPROC QueryDirectEx;
	FARPROC Query_Main;
	FARPROC Reg_FreeUpdateInfo;
	FARPROC Reg_GetValueEx;
	FARPROC Reg_ReadGlobalsEx;
	FARPROC Reg_ReadUpdateInfo;
	FARPROC Security_ContextListTimeout;
	FARPROC Send_AndRecvUdpWithParam;
	FARPROC Send_MessagePrivate;
	FARPROC Send_OpenTcpConnectionAndSend;
	FARPROC Socket_CacheCleanup;
	FARPROC Socket_CacheInit;
	FARPROC Socket_CleanupWinsock;
	FARPROC Socket_ClearMessageSockets;
	FARPROC Socket_CloseEx;
	FARPROC Socket_CloseMessageSockets;
	FARPROC Socket_Create;
	FARPROC Socket_CreateMulticast;
	FARPROC Socket_InitWinsock;
	FARPROC Socket_JoinMulticast;
	FARPROC Socket_RecvFrom;
	FARPROC Socket_SetMulticastInterface;
	FARPROC Socket_SetMulticastLoopBack;
	FARPROC Socket_SetTtl;
	FARPROC Socket_TcpListen;
	//FARPROC Trace_Reset;
	FARPROC Update_ReplaceAddressRecordsW;
	FARPROC Util_IsIp6Running;


	// DnsQueryEx and DnsCancelQuery are only present on win 8+ so they need stubs for older versions
	// they both take pointers to structs as parameters
	// (three for DnsQueryEx and one for DnsCancelQuery)
	// fortunately they should fit in registers so the stack doesn't matter
	static LONG WINAPI DnsQueryExStub()
	{
		return 9004; // DNS_ERROR_RCODE_NOT_IMPLEMENTED
	}
	static LONG WINAPI DnsCancelQueryStub()
	{
		return 9004; // DNS_ERROR_RCODE_NOT_IMPLEMENTED
	}

	void LoadOriginalLibrary(HMODULE module)
	{
		dll = module;
		shared.LoadOriginalLibrary(dll);

		BreakRecordsIntoBlob = GetProcAddress(dll, "BreakRecordsIntoBlob");
		CombineRecordsInBlob = GetProcAddress(dll, "CombineRecordsInBlob");
		DnsAcquireContextHandle_A = GetProcAddress(dll, "DnsAcquireContextHandle_A");
		DnsAcquireContextHandle_W = GetProcAddress(dll, "DnsAcquireContextHandle_W");
		DnsAllocateRecord = GetProcAddress(dll, "DnsAllocateRecord");
		DnsApiAlloc = GetProcAddress(dll, "DnsApiAlloc");
		DnsApiAllocZero = GetProcAddress(dll, "DnsApiAllocZero");
		DnsApiFree = GetProcAddress(dll, "DnsApiFree");
		DnsApiHeapReset = GetProcAddress(dll, "DnsApiHeapReset");
		DnsApiRealloc = GetProcAddress(dll, "DnsApiRealloc");
		DnsApiSetDebugGlobals = GetProcAddress(dll, "DnsApiSetDebugGlobals");
		DnsAsyncRegisterHostAddrs = GetProcAddress(dll, "DnsAsyncRegisterHostAddrs");
		DnsAsyncRegisterInit = GetProcAddress(dll, "DnsAsyncRegisterInit");
		DnsAsyncRegisterTerm = GetProcAddress(dll, "DnsAsyncRegisterTerm");
		DnsCancelQuery = GetProcAddress(dll, "DnsCancelQuery");
		DnsCopyStringEx = GetProcAddress(dll, "DnsCopyStringEx");
		DnsCreateReverseNameStringForIpAddress = GetProcAddress(dll, "DnsCreateReverseNameStringForIpAddress");
		DnsCreateStandardDnsNameCopy = GetProcAddress(dll, "DnsCreateStandardDnsNameCopy");
		DnsCreateStringCopy = GetProcAddress(dll, "DnsCreateStringCopy");
		DnsDhcpRegisterAddrs = GetProcAddress(dll, "DnsDhcpRegisterAddrs");
		DnsDhcpRegisterHostAddrs = GetProcAddress(dll, "DnsDhcpRegisterHostAddrs");
		DnsDhcpRegisterInit = GetProcAddress(dll, "DnsDhcpRegisterInit");
		DnsDhcpRegisterTerm = GetProcAddress(dll, "DnsDhcpRegisterTerm");
		DnsDhcpRemoveRegistrations = GetProcAddress(dll, "DnsDhcpRemoveRegistrations");
		DnsDhcpSrvRegisterHostAddr = GetProcAddress(dll, "DnsDhcpSrvRegisterHostAddr");
		//DnsDhcpSrvRegisterHostAddrEx = GetProcAddress(dll, "DnsDhcpSrvRegisterHostAddrEx");
		DnsDhcpSrvRegisterHostName = GetProcAddress(dll, "DnsDhcpSrvRegisterHostName");
		//DnsDhcpSrvRegisterHostNameEx = GetProcAddress(dll, "DnsDhcpSrvRegisterHostNameEx");
		DnsDhcpSrvRegisterInit = GetProcAddress(dll, "DnsDhcpSrvRegisterInit");
		DnsDhcpSrvRegisterInitialize = GetProcAddress(dll, "DnsDhcpSrvRegisterInitialize");
		DnsDhcpSrvRegisterTerm = GetProcAddress(dll, "DnsDhcpSrvRegisterTerm");
		DnsDowncaseDnsNameLabel = GetProcAddress(dll, "DnsDowncaseDnsNameLabel");
		DnsExtractRecordsFromMessage_UTF8 = GetProcAddress(dll, "DnsExtractRecordsFromMessage_UTF8");
		DnsExtractRecordsFromMessage_W = GetProcAddress(dll, "DnsExtractRecordsFromMessage_W");
		DnsFindAuthoritativeZone = GetProcAddress(dll, "DnsFindAuthoritativeZone");
		DnsFlushResolverCache = GetProcAddress(dll, "DnsFlushResolverCache");
		DnsFlushResolverCacheEntry_A = GetProcAddress(dll, "DnsFlushResolverCacheEntry_A");
		DnsFlushResolverCacheEntry_UTF8 = GetProcAddress(dll, "DnsFlushResolverCacheEntry_UTF8");
		DnsFlushResolverCacheEntry_W = GetProcAddress(dll, "DnsFlushResolverCacheEntry_W");
		DnsFree = GetProcAddress(dll, "DnsFree");
		DnsFreeConfigStructure = GetProcAddress(dll, "DnsFreeConfigStructure");
		//DnsFreePolicyConfig = GetProcAddress(dll, "DnsFreePolicyConfig");
		//DnsFreeProxyName = GetProcAddress(dll, "DnsFreeProxyName");
		DnsGetBufferLengthForStringCopy = GetProcAddress(dll, "DnsGetBufferLengthForStringCopy");
		DnsGetCacheDataTable = GetProcAddress(dll, "DnsGetCacheDataTable");
		DnsGetDnsServerList = GetProcAddress(dll, "DnsGetDnsServerList");
		DnsGetDomainName = GetProcAddress(dll, "DnsGetDomainName");
		DnsGetLastFailedUpdateInfo = GetProcAddress(dll, "DnsGetLastFailedUpdateInfo");
		//DnsGetPolicyTableInfo = GetProcAddress(dll, "DnsGetPolicyTableInfo");
		//DnsGetPolicyTableInfoPrivate = GetProcAddress(dll, "DnsGetPolicyTableInfoPrivate");
		DnsGetPrimaryDomainName_A = GetProcAddress(dll, "DnsGetPrimaryDomainName_A");
		//DnsGetProxyInfoPrivate = GetProcAddress(dll, "DnsGetProxyInfoPrivate");
		//DnsGetProxyInformation = GetProcAddress(dll, "DnsGetProxyInformation");
		DnsGlobals = GetProcAddress(dll, "DnsGlobals");
		DnsIpv6AddressToString = GetProcAddress(dll, "DnsIpv6AddressToString");
		DnsIpv6StringToAddress = GetProcAddress(dll, "DnsIpv6StringToAddress");
		DnsIsAMailboxType = GetProcAddress(dll, "DnsIsAMailboxType");
		DnsIsStatusRcode = GetProcAddress(dll, "DnsIsStatusRcode");
		DnsIsStringCountValidForTextType = GetProcAddress(dll, "DnsIsStringCountValidForTextType");
		DnsLogEvent = GetProcAddress(dll, "DnsLogEvent");
		DnsLogIn = GetProcAddress(dll, "DnsLogIn");
		DnsLogInit = GetProcAddress(dll, "DnsLogInit");
		DnsLogIt = GetProcAddress(dll, "DnsLogIt");
		DnsLogOut = GetProcAddress(dll, "DnsLogOut");
		DnsLogTime = GetProcAddress(dll, "DnsLogTime");
		DnsMapRcodeToStatus = GetProcAddress(dll, "DnsMapRcodeToStatus");
		DnsModifyRecordsInSet_A = GetProcAddress(dll, "DnsModifyRecordsInSet_A");
		DnsModifyRecordsInSet_UTF8 = GetProcAddress(dll, "DnsModifyRecordsInSet_UTF8");
		DnsModifyRecordsInSet_W = GetProcAddress(dll, "DnsModifyRecordsInSet_W");
		DnsNameCompareEx_A = GetProcAddress(dll, "DnsNameCompareEx_A");
		DnsNameCompareEx_UTF8 = GetProcAddress(dll, "DnsNameCompareEx_UTF8");
		DnsNameCompareEx_W = GetProcAddress(dll, "DnsNameCompareEx_W");
		DnsNameCompare_A = GetProcAddress(dll, "DnsNameCompare_A");
		DnsNameCompare_UTF8 = GetProcAddress(dll, "DnsNameCompare_UTF8");
		DnsNameCompare_W = GetProcAddress(dll, "DnsNameCompare_W");
		DnsNameCopy = GetProcAddress(dll, "DnsNameCopy");
		DnsNameCopyAllocate = GetProcAddress(dll, "DnsNameCopyAllocate");
		DnsNetworkInfo_CreateFromFAZ = GetProcAddress(dll, "DnsNetworkInfo_CreateFromFAZ");
		DnsNetworkInformation_CreateFromFAZ = GetProcAddress(dll, "DnsNetworkInformation_CreateFromFAZ");
		DnsNotifyResolver = GetProcAddress(dll, "DnsNotifyResolver");
		DnsNotifyResolverClusterIp = GetProcAddress(dll, "DnsNotifyResolverClusterIp");
		DnsNotifyResolverEx = GetProcAddress(dll, "DnsNotifyResolverEx");
		DnsQueryConfig = GetProcAddress(dll, "DnsQueryConfig");
		DnsQueryConfigAllocEx = GetProcAddress(dll, "DnsQueryConfigAllocEx");
		DnsQueryConfigDword = GetProcAddress(dll, "DnsQueryConfigDword");
		DnsQueryEx = GetProcAddress(dll, "DnsQueryEx");
		DnsQueryExA = GetProcAddress(dll, "DnsQueryExA");
		DnsQueryExUTF8 = GetProcAddress(dll, "DnsQueryExUTF8");
		DnsQueryExW = GetProcAddress(dll, "DnsQueryExW");
		DnsQuery_A = GetProcAddress(dll, "DnsQuery_A");
		DnsQuery_UTF8 = GetProcAddress(dll, "DnsQuery_UTF8");
		DnsQuery_W = GetProcAddress(dll, "DnsQuery_W");
		DnsRecordBuild_UTF8 = GetProcAddress(dll, "DnsRecordBuild_UTF8");
		DnsRecordBuild_W = GetProcAddress(dll, "DnsRecordBuild_W");
		DnsRecordCompare = GetProcAddress(dll, "DnsRecordCompare");
		DnsRecordCopyEx = GetProcAddress(dll, "DnsRecordCopyEx");
		DnsRecordListFree = GetProcAddress(dll, "DnsRecordListFree");
		DnsRecordSetCompare = GetProcAddress(dll, "DnsRecordSetCompare");
		DnsRecordSetCopyEx = GetProcAddress(dll, "DnsRecordSetCopyEx");
		DnsRecordSetDetach = GetProcAddress(dll, "DnsRecordSetDetach");
		DnsRecordStringForType = GetProcAddress(dll, "DnsRecordStringForType");
		DnsRecordStringForWritableType = GetProcAddress(dll, "DnsRecordStringForWritableType");
		DnsRecordTypeForName = GetProcAddress(dll, "DnsRecordTypeForName");
		DnsRegisterClusterAddress = GetProcAddress(dll, "DnsRegisterClusterAddress");
		DnsReleaseContextHandle = GetProcAddress(dll, "DnsReleaseContextHandle");
		DnsRemoveRegistrations = GetProcAddress(dll, "DnsRemoveRegistrations");
		DnsReplaceRecordSetA = GetProcAddress(dll, "DnsReplaceRecordSetA");
		DnsReplaceRecordSetUTF8 = GetProcAddress(dll, "DnsReplaceRecordSetUTF8");
		DnsReplaceRecordSetW = GetProcAddress(dll, "DnsReplaceRecordSetW");
		DnsResolverOp = GetProcAddress(dll, "DnsResolverOp");
		DnsScreenLocalAddrsForRegistration = GetProcAddress(dll, "DnsScreenLocalAddrsForRegistration");
		DnsSetConfigDword = GetProcAddress(dll, "DnsSetConfigDword");
		DnsStatusString = GetProcAddress(dll, "DnsStatusString");
		DnsStringCopyAllocateEx = GetProcAddress(dll, "DnsStringCopyAllocateEx");
		//DnsTraceServerConfig = GetProcAddress(dll, "DnsTraceServerConfig");
		DnsUnicodeToUtf8 = GetProcAddress(dll, "DnsUnicodeToUtf8");
		DnsUpdate = GetProcAddress(dll, "DnsUpdate");
		//DnsUpdateMachinePresence = GetProcAddress(dll, "DnsUpdateMachinePresence");
		DnsUpdateTest_A = GetProcAddress(dll, "DnsUpdateTest_A");
		DnsUpdateTest_UTF8 = GetProcAddress(dll, "DnsUpdateTest_UTF8");
		DnsUpdateTest_W = GetProcAddress(dll, "DnsUpdateTest_W");
		DnsUtf8ToUnicode = GetProcAddress(dll, "DnsUtf8ToUnicode");
		DnsValidateNameOrIp_TempW = GetProcAddress(dll, "DnsValidateNameOrIp_TempW");
		DnsValidateName_A = GetProcAddress(dll, "DnsValidateName_A");
		DnsValidateName_UTF8 = GetProcAddress(dll, "DnsValidateName_UTF8");
		DnsValidateName_W = GetProcAddress(dll, "DnsValidateName_W");
		//DnsValidateServerArray_A = GetProcAddress(dll, "DnsValidateServerArray_A");
		//DnsValidateServerArray_W = GetProcAddress(dll, "DnsValidateServerArray_W");
		//DnsValidateServerStatus = GetProcAddress(dll, "DnsValidateServerStatus");
		//DnsValidateServer_A = GetProcAddress(dll, "DnsValidateServer_A");
		//DnsValidateServer_W = GetProcAddress(dll, "DnsValidateServer_W");
		DnsValidateUtf8Byte = GetProcAddress(dll, "DnsValidateUtf8Byte");
		DnsWriteQuestionToBuffer_UTF8 = GetProcAddress(dll, "DnsWriteQuestionToBuffer_UTF8");
		DnsWriteQuestionToBuffer_W = GetProcAddress(dll, "DnsWriteQuestionToBuffer_W");
		DnsWriteReverseNameStringForIpAddress = GetProcAddress(dll, "DnsWriteReverseNameStringForIpAddress");
		Dns_AddRecordsToMessage = GetProcAddress(dll, "Dns_AddRecordsToMessage");
		Dns_AllocateMsgBuf = GetProcAddress(dll, "Dns_AllocateMsgBuf");
		Dns_BuildPacket = GetProcAddress(dll, "Dns_BuildPacket");
		Dns_CleanupWinsock = GetProcAddress(dll, "Dns_CleanupWinsock");
		Dns_CloseConnection = GetProcAddress(dll, "Dns_CloseConnection");
		Dns_CloseSocket = GetProcAddress(dll, "Dns_CloseSocket");
		Dns_CreateMulticastSocket = GetProcAddress(dll, "Dns_CreateMulticastSocket");
		Dns_CreateSocket = GetProcAddress(dll, "Dns_CreateSocket");
		Dns_CreateSocketEx = GetProcAddress(dll, "Dns_CreateSocketEx");
		Dns_ExtractRecordsFromMessage = GetProcAddress(dll, "Dns_ExtractRecordsFromMessage");
		Dns_FindAuthoritativeZoneLib = GetProcAddress(dll, "Dns_FindAuthoritativeZoneLib");
		Dns_FreeMsgBuf = GetProcAddress(dll, "Dns_FreeMsgBuf");
		Dns_GetRandomXid = GetProcAddress(dll, "Dns_GetRandomXid");
		Dns_InitializeMsgBuf = GetProcAddress(dll, "Dns_InitializeMsgBuf");
		Dns_InitializeMsgRemoteSockaddr = GetProcAddress(dll, "Dns_InitializeMsgRemoteSockaddr");
		Dns_InitializeWinsock = GetProcAddress(dll, "Dns_InitializeWinsock");
		Dns_OpenTcpConnectionAndSend = GetProcAddress(dll, "Dns_OpenTcpConnectionAndSend");
		Dns_ParseMessage = GetProcAddress(dll, "Dns_ParseMessage");
		Dns_ParsePacketRecord = GetProcAddress(dll, "Dns_ParsePacketRecord");
		Dns_PingAdapterServers = GetProcAddress(dll, "Dns_PingAdapterServers");
		Dns_ReadPacketName = GetProcAddress(dll, "Dns_ReadPacketName");
		Dns_ReadPacketNameAllocate = GetProcAddress(dll, "Dns_ReadPacketNameAllocate");
		Dns_ReadRecordStructureFromPacket = GetProcAddress(dll, "Dns_ReadRecordStructureFromPacket");
		Dns_RecvTcp = GetProcAddress(dll, "Dns_RecvTcp");
		Dns_ResetNetworkInfo = GetProcAddress(dll, "Dns_ResetNetworkInfo");
		Dns_SendAndRecvUdp = GetProcAddress(dll, "Dns_SendAndRecvUdp");
		Dns_SendEx = GetProcAddress(dll, "Dns_SendEx");
		Dns_SetRecordDatalength = GetProcAddress(dll, "Dns_SetRecordDatalength");
		Dns_SetRecordsSection = GetProcAddress(dll, "Dns_SetRecordsSection");
		Dns_SetRecordsTtl = GetProcAddress(dll, "Dns_SetRecordsTtl");
		Dns_SkipPacketName = GetProcAddress(dll, "Dns_SkipPacketName");
		Dns_SkipToRecord = GetProcAddress(dll, "Dns_SkipToRecord");
		Dns_UpdateLib = GetProcAddress(dll, "Dns_UpdateLib");
		Dns_UpdateLibEx = GetProcAddress(dll, "Dns_UpdateLibEx");
		Dns_WriteDottedNameToPacket = GetProcAddress(dll, "Dns_WriteDottedNameToPacket");
		Dns_WriteQuestionToMessage = GetProcAddress(dll, "Dns_WriteQuestionToMessage");
		Dns_WriteRecordStructureToPacketEx = GetProcAddress(dll, "Dns_WriteRecordStructureToPacketEx");
		ExtraInfo_Init = GetProcAddress(dll, "ExtraInfo_Init");
		Faz_AreServerListsInSameNameSpace = GetProcAddress(dll, "Faz_AreServerListsInSameNameSpace");
		//FlushDnsPolicyUnreachableStatus = GetProcAddress(dll, "FlushDnsPolicyUnreachableStatus");
		GetCurrentTimeInSeconds = GetProcAddress(dll, "GetCurrentTimeInSeconds");
		HostsFile_Close = GetProcAddress(dll, "HostsFile_Close");
		HostsFile_Open = GetProcAddress(dll, "HostsFile_Open");
		HostsFile_ReadLine = GetProcAddress(dll, "HostsFile_ReadLine");
		IpHelp_IsAddrOnLink = GetProcAddress(dll, "IpHelp_IsAddrOnLink");
		Local_GetRecordsForLocalName = GetProcAddress(dll, "Local_GetRecordsForLocalName");
		Local_GetRecordsForLocalNameEx = GetProcAddress(dll, "Local_GetRecordsForLocalNameEx");
		NetInfo_Build = GetProcAddress(dll, "NetInfo_Build");
		NetInfo_Clean = GetProcAddress(dll, "NetInfo_Clean");
		NetInfo_Copy = GetProcAddress(dll, "NetInfo_Copy");
		NetInfo_Free = GetProcAddress(dll, "NetInfo_Free");
		NetInfo_GetAdapterByAddress = GetProcAddress(dll, "NetInfo_GetAdapterByAddress");
		NetInfo_GetAdapterByInterfaceIndex = GetProcAddress(dll, "NetInfo_GetAdapterByInterfaceIndex");
		NetInfo_GetAdapterByName = GetProcAddress(dll, "NetInfo_GetAdapterByName");
		NetInfo_IsAddrConfig = GetProcAddress(dll, "NetInfo_IsAddrConfig");
		NetInfo_IsForUpdate = GetProcAddress(dll, "NetInfo_IsForUpdate");
		NetInfo_ResetServerPriorities = GetProcAddress(dll, "NetInfo_ResetServerPriorities");
		//NetInfo_UpdateServerReachability = GetProcAddress(dll, "NetInfo_UpdateServerReachability");
		QueryDirectEx = GetProcAddress(dll, "QueryDirectEx");
		Query_Main = GetProcAddress(dll, "Query_Main");
		Reg_FreeUpdateInfo = GetProcAddress(dll, "Reg_FreeUpdateInfo");
		Reg_GetValueEx = GetProcAddress(dll, "Reg_GetValueEx");
		Reg_ReadGlobalsEx = GetProcAddress(dll, "Reg_ReadGlobalsEx");
		Reg_ReadUpdateInfo = GetProcAddress(dll, "Reg_ReadUpdateInfo");
		Security_ContextListTimeout = GetProcAddress(dll, "Security_ContextListTimeout");
		Send_AndRecvUdpWithParam = GetProcAddress(dll, "Send_AndRecvUdpWithParam");
		Send_MessagePrivate = GetProcAddress(dll, "Send_MessagePrivate");
		Send_OpenTcpConnectionAndSend = GetProcAddress(dll, "Send_OpenTcpConnectionAndSend");
		Socket_CacheCleanup = GetProcAddress(dll, "Socket_CacheCleanup");
		Socket_CacheInit = GetProcAddress(dll, "Socket_CacheInit");
		Socket_CleanupWinsock = GetProcAddress(dll, "Socket_CleanupWinsock");
		Socket_ClearMessageSockets = GetProcAddress(dll, "Socket_ClearMessageSockets");
		Socket_CloseEx = GetProcAddress(dll, "Socket_CloseEx");
		Socket_CloseMessageSockets = GetProcAddress(dll, "Socket_CloseMessageSockets");
		Socket_Create = GetProcAddress(dll, "Socket_Create");
		Socket_CreateMulticast = GetProcAddress(dll, "Socket_CreateMulticast");
		Socket_InitWinsock = GetProcAddress(dll, "Socket_InitWinsock");
		Socket_JoinMulticast = GetProcAddress(dll, "Socket_JoinMulticast");
		Socket_RecvFrom = GetProcAddress(dll, "Socket_RecvFrom");
		Socket_SetMulticastInterface = GetProcAddress(dll, "Socket_SetMulticastInterface");
		Socket_SetMulticastLoopBack = GetProcAddress(dll, "Socket_SetMulticastLoopBack");
		Socket_SetTtl = GetProcAddress(dll, "Socket_SetTtl");
		Socket_TcpListen = GetProcAddress(dll, "Socket_TcpListen");
		//Trace_Reset = GetProcAddress(dll, "Trace_Reset");
		Update_ReplaceAddressRecordsW = GetProcAddress(dll, "Update_ReplaceAddressRecordsW");
		Util_IsIp6Running = GetProcAddress(dll, "Util_IsIp6Running");


		// if entry points aren't found, GetProcAddress should return null
		if (DnsQueryEx == NULL) { DnsQueryEx = (FARPROC)& DnsQueryExStub; };
		if (DnsCancelQuery == NULL) { DnsCancelQuery = (FARPROC)& DnsCancelQueryStub; };
	}
} dnsapi;


#pragma runtime_checks( "", off )

#ifdef _DEBUG
#pragma message ("You are compiling the code in Debug - be warned that wrappers for export functions may not have correct code generated")
#endif

// not sure if these all work -- there's a very real possibility some of these will need more full definitions to not break stack parameters
// (see https://github.com/ThirteenAG/Ultimate-ASI-Loader/blob/master/source/dllmain.h#L2870 for reference)
void _BreakRecordsIntoBlob() { dnsapi.BreakRecordsIntoBlob(); }
void _CombineRecordsInBlob() { dnsapi.CombineRecordsInBlob(); }
void _DnsAcquireContextHandle_A() { dnsapi.DnsAcquireContextHandle_A(); }
void _DnsAcquireContextHandle_W() { dnsapi.DnsAcquireContextHandle_W(); }
void _DnsAllocateRecord() { dnsapi.DnsAllocateRecord(); }
void _DnsApiAlloc() { dnsapi.DnsApiAlloc(); }
void _DnsApiAllocZero() { dnsapi.DnsApiAllocZero(); }
void _DnsApiFree() { dnsapi.DnsApiFree(); }
void _DnsApiHeapReset() { dnsapi.DnsApiHeapReset(); }
void _DnsApiRealloc() { dnsapi.DnsApiRealloc(); }
void _DnsApiSetDebugGlobals() { dnsapi.DnsApiSetDebugGlobals(); }
void _DnsAsyncRegisterHostAddrs() { dnsapi.DnsAsyncRegisterHostAddrs(); }
void _DnsAsyncRegisterInit() { dnsapi.DnsAsyncRegisterInit(); }
void _DnsAsyncRegisterTerm() { dnsapi.DnsAsyncRegisterTerm(); }
void _DnsCancelQuery() { dnsapi.DnsCancelQuery(); }
void _DnsCopyStringEx() { dnsapi.DnsCopyStringEx(); }
void _DnsCreateReverseNameStringForIpAddress() { dnsapi.DnsCreateReverseNameStringForIpAddress(); }
void _DnsCreateStandardDnsNameCopy() { dnsapi.DnsCreateStandardDnsNameCopy(); }
void _DnsCreateStringCopy() { dnsapi.DnsCreateStringCopy(); }
void _DnsDhcpRegisterAddrs() { dnsapi.DnsDhcpRegisterAddrs(); }
void _DnsDhcpRegisterHostAddrs() { dnsapi.DnsDhcpRegisterHostAddrs(); }
void _DnsDhcpRegisterInit() { dnsapi.DnsDhcpRegisterInit(); }
void _DnsDhcpRegisterTerm() { dnsapi.DnsDhcpRegisterTerm(); }
void _DnsDhcpRemoveRegistrations() { dnsapi.DnsDhcpRemoveRegistrations(); }
void _DnsDhcpSrvRegisterHostAddr() { dnsapi.DnsDhcpSrvRegisterHostAddr(); }
//void _DnsDhcpSrvRegisterHostAddrEx() { dnsapi.DnsDhcpSrvRegisterHostAddrEx(); }
void _DnsDhcpSrvRegisterHostName() { dnsapi.DnsDhcpSrvRegisterHostName(); }
//void _DnsDhcpSrvRegisterHostNameEx() { dnsapi.DnsDhcpSrvRegisterHostNameEx(); }
void _DnsDhcpSrvRegisterInit() { dnsapi.DnsDhcpSrvRegisterInit(); }
void _DnsDhcpSrvRegisterInitialize() { dnsapi.DnsDhcpSrvRegisterInitialize(); }
void _DnsDhcpSrvRegisterTerm() { dnsapi.DnsDhcpSrvRegisterTerm(); }
void _DnsDowncaseDnsNameLabel() { dnsapi.DnsDowncaseDnsNameLabel(); }
void _DnsExtractRecordsFromMessage_UTF8() { dnsapi.DnsExtractRecordsFromMessage_UTF8(); }
void _DnsExtractRecordsFromMessage_W() { dnsapi.DnsExtractRecordsFromMessage_W(); }
void _DnsFindAuthoritativeZone() { dnsapi.DnsFindAuthoritativeZone(); }
void _DnsFlushResolverCache() { dnsapi.DnsFlushResolverCache(); }
void _DnsFlushResolverCacheEntry_A() { dnsapi.DnsFlushResolverCacheEntry_A(); }
void _DnsFlushResolverCacheEntry_UTF8() { dnsapi.DnsFlushResolverCacheEntry_UTF8(); }
void _DnsFlushResolverCacheEntry_W() { dnsapi.DnsFlushResolverCacheEntry_W(); }
void _DnsFree() { dnsapi.DnsFree(); }
void _DnsFreeConfigStructure() { dnsapi.DnsFreeConfigStructure(); }
//void _DnsFreePolicyConfig() { dnsapi.DnsFreePolicyConfig(); }
//void _DnsFreeProxyName() { dnsapi.DnsFreeProxyName(); }
void _DnsGetBufferLengthForStringCopy() { dnsapi.DnsGetBufferLengthForStringCopy(); }
void _DnsGetCacheDataTable() { dnsapi.DnsGetCacheDataTable(); }
void _DnsGetDnsServerList() { dnsapi.DnsGetDnsServerList(); }
void _DnsGetDomainName() { dnsapi.DnsGetDomainName(); }
void _DnsGetLastFailedUpdateInfo() { dnsapi.DnsGetLastFailedUpdateInfo(); }
//void _DnsGetPolicyTableInfo() { dnsapi.DnsGetPolicyTableInfo(); }
//void _DnsGetPolicyTableInfoPrivate() { dnsapi.DnsGetPolicyTableInfoPrivate(); }
void _DnsGetPrimaryDomainName_A() { dnsapi.DnsGetPrimaryDomainName_A(); }
//void _DnsGetProxyInfoPrivate() { dnsapi.DnsGetProxyInfoPrivate(); }
//void _DnsGetProxyInformation() { dnsapi.DnsGetProxyInformation(); }
void _DnsGlobals() { dnsapi.DnsGlobals(); }
void _DnsIpv6AddressToString() { dnsapi.DnsIpv6AddressToString(); }
void _DnsIpv6StringToAddress() { dnsapi.DnsIpv6StringToAddress(); }
void _DnsIsAMailboxType() { dnsapi.DnsIsAMailboxType(); }
void _DnsIsStatusRcode() { dnsapi.DnsIsStatusRcode(); }
void _DnsIsStringCountValidForTextType() { dnsapi.DnsIsStringCountValidForTextType(); }
void _DnsLogEvent() { dnsapi.DnsLogEvent(); }
void _DnsLogIn() { dnsapi.DnsLogIn(); }
void _DnsLogInit() { dnsapi.DnsLogInit(); }
void _DnsLogIt() { dnsapi.DnsLogIt(); }
void _DnsLogOut() { dnsapi.DnsLogOut(); }
void _DnsLogTime() { dnsapi.DnsLogTime(); }
void _DnsMapRcodeToStatus() { dnsapi.DnsMapRcodeToStatus(); }
void _DnsModifyRecordsInSet_A() { dnsapi.DnsModifyRecordsInSet_A(); }
void _DnsModifyRecordsInSet_UTF8() { dnsapi.DnsModifyRecordsInSet_UTF8(); }
void _DnsModifyRecordsInSet_W() { dnsapi.DnsModifyRecordsInSet_W(); }
void _DnsNameCompareEx_A() { dnsapi.DnsNameCompareEx_A(); }
void _DnsNameCompareEx_UTF8() { dnsapi.DnsNameCompareEx_UTF8(); }
void _DnsNameCompareEx_W() { dnsapi.DnsNameCompareEx_W(); }
void _DnsNameCompare_A() { dnsapi.DnsNameCompare_A(); }
void _DnsNameCompare_UTF8() { dnsapi.DnsNameCompare_UTF8(); }
void _DnsNameCompare_W() { dnsapi.DnsNameCompare_W(); }
void _DnsNameCopy() { dnsapi.DnsNameCopy(); }
void _DnsNameCopyAllocate() { dnsapi.DnsNameCopyAllocate(); }
void _DnsNetworkInfo_CreateFromFAZ() { dnsapi.DnsNetworkInfo_CreateFromFAZ(); }
void _DnsNetworkInformation_CreateFromFAZ() { dnsapi.DnsNetworkInformation_CreateFromFAZ(); }
void _DnsNotifyResolver() { dnsapi.DnsNotifyResolver(); }
void _DnsNotifyResolverClusterIp() { dnsapi.DnsNotifyResolverClusterIp(); }
void _DnsNotifyResolverEx() { dnsapi.DnsNotifyResolverEx(); }
void _DnsQueryConfig() { dnsapi.DnsQueryConfig(); }
void _DnsQueryConfigAllocEx() { dnsapi.DnsQueryConfigAllocEx(); }
void _DnsQueryConfigDword() { dnsapi.DnsQueryConfigDword(); }
void _DnsQueryEx() { dnsapi.DnsQueryEx(); }
void _DnsQueryExA() { dnsapi.DnsQueryExA(); }
void _DnsQueryExUTF8() { dnsapi.DnsQueryExUTF8(); }
void _DnsQueryExW() { dnsapi.DnsQueryExW(); }
void _DnsQuery_A() { dnsapi.DnsQuery_A(); }
void _DnsQuery_UTF8() { dnsapi.DnsQuery_UTF8(); }
void _DnsQuery_W() { dnsapi.DnsQuery_W(); }
void _DnsRecordBuild_UTF8() { dnsapi.DnsRecordBuild_UTF8(); }
void _DnsRecordBuild_W() { dnsapi.DnsRecordBuild_W(); }
void _DnsRecordCompare() { dnsapi.DnsRecordCompare(); }
void _DnsRecordCopyEx() { dnsapi.DnsRecordCopyEx(); }
void _DnsRecordListFree() { dnsapi.DnsRecordListFree(); }
void _DnsRecordSetCompare() { dnsapi.DnsRecordSetCompare(); }
void _DnsRecordSetCopyEx() { dnsapi.DnsRecordSetCopyEx(); }
void _DnsRecordSetDetach() { dnsapi.DnsRecordSetDetach(); }
void _DnsRecordStringForType() { dnsapi.DnsRecordStringForType(); }
void _DnsRecordStringForWritableType() { dnsapi.DnsRecordStringForWritableType(); }
void _DnsRecordTypeForName() { dnsapi.DnsRecordTypeForName(); }
void _DnsRegisterClusterAddress() { dnsapi.DnsRegisterClusterAddress(); }
void _DnsReleaseContextHandle() { dnsapi.DnsReleaseContextHandle(); }
void _DnsRemoveRegistrations() { dnsapi.DnsRemoveRegistrations(); }
void _DnsReplaceRecordSetA() { dnsapi.DnsReplaceRecordSetA(); }
void _DnsReplaceRecordSetUTF8() { dnsapi.DnsReplaceRecordSetUTF8(); }
void _DnsReplaceRecordSetW() { dnsapi.DnsReplaceRecordSetW(); }
void _DnsResolverOp() { dnsapi.DnsResolverOp(); }
void _DnsScreenLocalAddrsForRegistration() { dnsapi.DnsScreenLocalAddrsForRegistration(); }
void _DnsSetConfigDword() { dnsapi.DnsSetConfigDword(); }
void _DnsStatusString() { dnsapi.DnsStatusString(); }
void _DnsStringCopyAllocateEx() { dnsapi.DnsStringCopyAllocateEx(); }
//void _DnsTraceServerConfig() { dnsapi.DnsTraceServerConfig(); }
void _DnsUnicodeToUtf8() { dnsapi.DnsUnicodeToUtf8(); }
void _DnsUpdate() { dnsapi.DnsUpdate(); }
//void _DnsUpdateMachinePresence() { dnsapi.DnsUpdateMachinePresence(); }
void _DnsUpdateTest_A() { dnsapi.DnsUpdateTest_A(); }
void _DnsUpdateTest_UTF8() { dnsapi.DnsUpdateTest_UTF8(); }
void _DnsUpdateTest_W() { dnsapi.DnsUpdateTest_W(); }
void _DnsUtf8ToUnicode() { dnsapi.DnsUtf8ToUnicode(); }
void _DnsValidateNameOrIp_TempW() { dnsapi.DnsValidateNameOrIp_TempW(); }
void _DnsValidateName_A() { dnsapi.DnsValidateName_A(); }
void _DnsValidateName_UTF8() { dnsapi.DnsValidateName_UTF8(); }
void _DnsValidateName_W() { dnsapi.DnsValidateName_W(); }
//void _DnsValidateServerArray_A() { dnsapi.DnsValidateServerArray_A(); }
//void _DnsValidateServerArray_W() { dnsapi.DnsValidateServerArray_W(); }
//void _DnsValidateServerStatus() { dnsapi.DnsValidateServerStatus(); }
//void _DnsValidateServer_A() { dnsapi.DnsValidateServer_A(); }
//void _DnsValidateServer_W() { dnsapi.DnsValidateServer_W(); }
void _DnsValidateUtf8Byte() { dnsapi.DnsValidateUtf8Byte(); }
void _DnsWriteQuestionToBuffer_UTF8() { dnsapi.DnsWriteQuestionToBuffer_UTF8(); }
void _DnsWriteQuestionToBuffer_W() { dnsapi.DnsWriteQuestionToBuffer_W(); }
void _DnsWriteReverseNameStringForIpAddress() { dnsapi.DnsWriteReverseNameStringForIpAddress(); }
void _Dns_AddRecordsToMessage() { dnsapi.Dns_AddRecordsToMessage(); }
void _Dns_AllocateMsgBuf() { dnsapi.Dns_AllocateMsgBuf(); }
void _Dns_BuildPacket() { dnsapi.Dns_BuildPacket(); }
void _Dns_CleanupWinsock() { dnsapi.Dns_CleanupWinsock(); }
void _Dns_CloseConnection() { dnsapi.Dns_CloseConnection(); }
void _Dns_CloseSocket() { dnsapi.Dns_CloseSocket(); }
void _Dns_CreateMulticastSocket() { dnsapi.Dns_CreateMulticastSocket(); }
void _Dns_CreateSocket() { dnsapi.Dns_CreateSocket(); }
void _Dns_CreateSocketEx() { dnsapi.Dns_CreateSocketEx(); }
void _Dns_ExtractRecordsFromMessage() { dnsapi.Dns_ExtractRecordsFromMessage(); }
void _Dns_FindAuthoritativeZoneLib() { dnsapi.Dns_FindAuthoritativeZoneLib(); }
void _Dns_FreeMsgBuf() { dnsapi.Dns_FreeMsgBuf(); }
void _Dns_GetRandomXid() { dnsapi.Dns_GetRandomXid(); }
void _Dns_InitializeMsgBuf() { dnsapi.Dns_InitializeMsgBuf(); }
void _Dns_InitializeMsgRemoteSockaddr() { dnsapi.Dns_InitializeMsgRemoteSockaddr(); }
void _Dns_InitializeWinsock() { dnsapi.Dns_InitializeWinsock(); }
void _Dns_OpenTcpConnectionAndSend() { dnsapi.Dns_OpenTcpConnectionAndSend(); }
void _Dns_ParseMessage() { dnsapi.Dns_ParseMessage(); }
void _Dns_ParsePacketRecord() { dnsapi.Dns_ParsePacketRecord(); }
void _Dns_PingAdapterServers() { dnsapi.Dns_PingAdapterServers(); }
void _Dns_ReadPacketName() { dnsapi.Dns_ReadPacketName(); }
void _Dns_ReadPacketNameAllocate() { dnsapi.Dns_ReadPacketNameAllocate(); }
void _Dns_ReadRecordStructureFromPacket() { dnsapi.Dns_ReadRecordStructureFromPacket(); }
void _Dns_RecvTcp() { dnsapi.Dns_RecvTcp(); }
void _Dns_ResetNetworkInfo() { dnsapi.Dns_ResetNetworkInfo(); }
void _Dns_SendAndRecvUdp() { dnsapi.Dns_SendAndRecvUdp(); }
void _Dns_SendEx() { dnsapi.Dns_SendEx(); }
void _Dns_SetRecordDatalength() { dnsapi.Dns_SetRecordDatalength(); }
void _Dns_SetRecordsSection() { dnsapi.Dns_SetRecordsSection(); }
void _Dns_SetRecordsTtl() { dnsapi.Dns_SetRecordsTtl(); }
void _Dns_SkipPacketName() { dnsapi.Dns_SkipPacketName(); }
void _Dns_SkipToRecord() { dnsapi.Dns_SkipToRecord(); }
void _Dns_UpdateLib() { dnsapi.Dns_UpdateLib(); }
void _Dns_UpdateLibEx() { dnsapi.Dns_UpdateLibEx(); }
void _Dns_WriteDottedNameToPacket() { dnsapi.Dns_WriteDottedNameToPacket(); }
void _Dns_WriteQuestionToMessage() { dnsapi.Dns_WriteQuestionToMessage(); }
void _Dns_WriteRecordStructureToPacketEx() { dnsapi.Dns_WriteRecordStructureToPacketEx(); }
void _ExtraInfo_Init() { dnsapi.ExtraInfo_Init(); }
void _Faz_AreServerListsInSameNameSpace() { dnsapi.Faz_AreServerListsInSameNameSpace(); }
//void _FlushDnsPolicyUnreachableStatus() { dnsapi.FlushDnsPolicyUnreachableStatus(); }
void _GetCurrentTimeInSeconds() { dnsapi.GetCurrentTimeInSeconds(); }
void _HostsFile_Close() { dnsapi.HostsFile_Close(); }
void _HostsFile_Open() { dnsapi.HostsFile_Open(); }
void _HostsFile_ReadLine() { dnsapi.HostsFile_ReadLine(); }
void _IpHelp_IsAddrOnLink() { dnsapi.IpHelp_IsAddrOnLink(); }
void _Local_GetRecordsForLocalName() { dnsapi.Local_GetRecordsForLocalName(); }
void _Local_GetRecordsForLocalNameEx() { dnsapi.Local_GetRecordsForLocalNameEx(); }
void _NetInfo_Build() { dnsapi.NetInfo_Build(); }
void _NetInfo_Clean() { dnsapi.NetInfo_Clean(); }
void _NetInfo_Copy() { dnsapi.NetInfo_Copy(); }
void _NetInfo_Free() { dnsapi.NetInfo_Free(); }
void _NetInfo_GetAdapterByAddress() { dnsapi.NetInfo_GetAdapterByAddress(); }
void _NetInfo_GetAdapterByInterfaceIndex() { dnsapi.NetInfo_GetAdapterByInterfaceIndex(); }
void _NetInfo_GetAdapterByName() { dnsapi.NetInfo_GetAdapterByName(); }
void _NetInfo_IsAddrConfig() { dnsapi.NetInfo_IsAddrConfig(); }
void _NetInfo_IsForUpdate() { dnsapi.NetInfo_IsForUpdate(); }
void _NetInfo_ResetServerPriorities() { dnsapi.NetInfo_ResetServerPriorities(); }
//void _NetInfo_UpdateServerReachability() { dnsapi.NetInfo_UpdateServerReachability(); }
void _QueryDirectEx() { dnsapi.QueryDirectEx(); }
void _Query_Main() { dnsapi.Query_Main(); }
void _Reg_FreeUpdateInfo() { dnsapi.Reg_FreeUpdateInfo(); }
void _Reg_GetValueEx() { dnsapi.Reg_GetValueEx(); }
void _Reg_ReadGlobalsEx() { dnsapi.Reg_ReadGlobalsEx(); }
void _Reg_ReadUpdateInfo() { dnsapi.Reg_ReadUpdateInfo(); }
void _Security_ContextListTimeout() { dnsapi.Security_ContextListTimeout(); }
void _Send_AndRecvUdpWithParam() { dnsapi.Send_AndRecvUdpWithParam(); }
void _Send_MessagePrivate() { dnsapi.Send_MessagePrivate(); }
void _Send_OpenTcpConnectionAndSend() { dnsapi.Send_OpenTcpConnectionAndSend(); }
void _Socket_CacheCleanup() { dnsapi.Socket_CacheCleanup(); }
void _Socket_CacheInit() { dnsapi.Socket_CacheInit(); }
void _Socket_CleanupWinsock() { dnsapi.Socket_CleanupWinsock(); }
void _Socket_ClearMessageSockets() { dnsapi.Socket_ClearMessageSockets(); }
void _Socket_CloseEx() { dnsapi.Socket_CloseEx(); }
void _Socket_CloseMessageSockets() { dnsapi.Socket_CloseMessageSockets(); }
void _Socket_Create() { dnsapi.Socket_Create(); }
void _Socket_CreateMulticast() { dnsapi.Socket_CreateMulticast(); }
void _Socket_InitWinsock() { dnsapi.Socket_InitWinsock(); }
void _Socket_JoinMulticast() { dnsapi.Socket_JoinMulticast(); }
void _Socket_RecvFrom() { dnsapi.Socket_RecvFrom(); }
void _Socket_SetMulticastInterface() { dnsapi.Socket_SetMulticastInterface(); }
void _Socket_SetMulticastLoopBack() { dnsapi.Socket_SetMulticastLoopBack(); }
void _Socket_SetTtl() { dnsapi.Socket_SetTtl(); }
void _Socket_TcpListen() { dnsapi.Socket_TcpListen(); }
//void _Trace_Reset() { dnsapi.Trace_Reset(); }
void _Update_ReplaceAddressRecordsW() { dnsapi.Update_ReplaceAddressRecordsW(); }
void _Util_IsIp6Running() { dnsapi.Util_IsIp6Running(); }


void _DllRegisterServer() { shared.DllRegisterServer(); }
void _DllUnregisterServer() { shared.DllUnregisterServer(); }
void _DllCanUnloadNow() { shared.DllCanUnloadNow(); }
void _DllGetClassObject() { shared.DllGetClassObject(); }

#pragma runtime_checks( "", restore )