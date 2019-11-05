#pragma once
#include <stdint.h>

struct PlayerData
{
	int8_t use_card;
	int8_t freeplay;
	int8_t field_2;
	int8_t field_3;
	int32_t card_type;
	int32_t field_8;
	int32_t field_C;
	int32_t field_10;
	int32_t field_14;
	int32_t field_18;
	int32_t field_1C;
	int32_t field_20;
	int32_t field_24;
	int32_t field_28;
	int32_t field_2C;
	int32_t field_30;
	int32_t field_34;
	int32_t field_38;
	int32_t field_3C;
	int32_t field_40;
	int32_t field_44;
	int32_t field_48;
	int32_t field_4C;
	int32_t field_50;
	int32_t field_54;
	int32_t field_58;
	int32_t field_5C;
	int32_t field_60;
	int32_t field_64;
	int32_t field_68;
	int32_t field_6C;
	int32_t field_70;
	int32_t field_74;
	int32_t field_78;
	int32_t field_7C;
	int32_t field_80;
	int32_t field_84;
	int32_t field_88;
	int32_t field_8C;
	int32_t field_90;
	int32_t field_94;
	int32_t field_98;
	int32_t field_9C;
	int32_t field_A0;
	int32_t field_A4;
	int32_t field_A8;
	int32_t field_AC;
	int32_t field_B0;
	int32_t field_B4;
	int32_t field_B8;
	int32_t field_BC;
	int32_t field_C0;
	int32_t field_C4;
	int32_t field_C8;
	int32_t field_CC;
	int32_t play_data_id;
	int32_t accept_index;
	int32_t start_index;
	int32_t field_DC;
	char* player_name;
	int32_t field_E8;
	int32_t field_EC;
	int32_t field_F0;
	int32_t field_F4;
	int32_t field_F8;
	int32_t field_FC;
	char* level_name;
	int32_t field_108;
	int32_t field_10C;
	int32_t field_110;
	int32_t field_114;
	int32_t field_118;
	int32_t field_11C;
	int32_t level;
	int32_t level_plate_id;
	int32_t level_plate_effect;
	int32_t vocaloid_point;
	int32_t hp_vol;
	int32_t act_toggle;
	int32_t act_vol;
	int32_t act_slide_vol;
	int32_t field_140;
	int32_t field_144;
	int32_t field_148;
	int32_t field_14C;
	int32_t field_150;
	int32_t field_154;
	int32_t field_158;
	int32_t field_15C;
	int32_t field_160;
	int32_t field_164;
	int32_t field_168;
	int32_t field_16C;
	int32_t field_170; //170
	int32_t field_174;
	int32_t field_178;
	int32_t field_17C;
	int32_t field_180;
	int32_t field_184; //184
	int32_t field_188;
	int32_t field_18C;
	int32_t field_190;
	int32_t field_194;
	int32_t field_198; //198
	int32_t field_19C;
	int32_t field_1A0;
	int32_t field_1A4;
	int32_t field_1A8;
	int32_t field_1AC; //1AC
	int32_t field_1B0;
	int32_t field_1B4;
	int32_t field_1B8;
	int32_t field_1BC;
	int32_t module_equip0; //1C0
	int32_t module_equip1; //1C4
	int32_t module_equip2; //1C8
	int32_t module_equip3; //1CC
	int32_t module_equip4; //1D0
	int32_t module_equip5; //1D4
	int32_t module_equip_cmn0; //1D8
	int32_t module_equip_cmn1;//1DC
	int32_t module_equip_cmn2;//1E0
	int32_t module_equip_cmn3;//1E4
	int32_t module_equip_cmn4;//1E8
	int32_t module_equip_cmn5;//1EC
	int32_t field_1F0;//1F0
	int32_t field_1F4;
	int32_t field_1F8;
	int32_t field_1FC;
	int32_t field_200;
	int32_t field_204;
	int32_t field_208;
	int32_t field_20C;
	int32_t field_210;
	int32_t field_214;
	int32_t field_218;
	int32_t field_21C;
	int32_t field_220;
	int32_t field_224;
	int32_t field_228;
	int32_t field_22C;
	int32_t field_230;
	int32_t field_234;
	int32_t field_238;
	int32_t field_23C;
	int32_t module_hist; //240
	int32_t field_244;
	int32_t field_248;
	int32_t field_24C;
	int32_t field_250;
	int32_t field_254;
	int32_t field_258;
	int32_t field_25C;
	int32_t field_260;
	int32_t field_264;
	int32_t field_268;
	int32_t field_26C;
	int32_t field_270;
	int32_t field_274;
	int32_t field_278;
	int32_t field_27C;
	int32_t field_280;
	int32_t field_284;
	int32_t field_288;
	int32_t field_28C;
	int32_t field_290;
	int32_t field_294;
	int32_t field_298;
	int32_t field_29C;
	int32_t field_2A0;
	int32_t field_2A4;
	int32_t field_2A8;
	int32_t field_2AC;
	int8_t use_pv_module_equip;
	int8_t ch_pv_module_equip;
	int8_t field_2B2;
	int8_t field_2B3;
	int32_t module_filter_kind;
	int32_t field_2B8;
	int32_t field_2BC;
	int32_t field_2C0;
	int32_t field_2C4;
	int32_t field_2C8;
	int32_t field_2CC;
	int32_t field_2D0;
	int32_t field_2D4;
	int32_t field_2D8;
	int32_t field_2DC;
	int32_t field_2E0;
	int32_t field_2E4;
	int32_t field_2E8;
	int32_t field_2EC;
	int32_t field_2F0;
	int32_t field_2F4;
	int32_t field_2F8;
	int32_t field_2FC;
	int32_t field_300;
	int32_t field_304;
	int32_t field_308;
	int32_t field_30C;
	int32_t field_310;
	int32_t field_314;
	int32_t field_318;
	int32_t field_31C;
	int32_t field_320;
	int32_t field_324;
	int32_t field_328;
	int32_t field_32C;
	int32_t field_330;
	int32_t field_334;
	int32_t field_338;
	int32_t field_33C;
	int32_t field_340;
	int32_t field_344;
	int32_t field_348;
	int32_t field_34C;
	int32_t field_350;
	int32_t field_354;
	int32_t field_358;
	int32_t field_35C;
	int32_t field_360;
	int32_t field_364;
	int32_t field_368;
	int32_t field_36C;
	int32_t field_370;
	int32_t field_374;
	int32_t field_378;
	int32_t field_37C;
	int32_t field_380;
	int32_t field_384;
	int32_t field_388;
	int32_t field_38C;
	int32_t field_390;
	int32_t field_394;
	int32_t field_398;
	int32_t field_39C;
	int32_t field_3A0;
	int32_t field_3A4;
	int32_t field_3A8;
	int32_t field_3AC;
	int32_t field_3B0;
	int32_t field_3B4;
	int32_t field_3B8;
	int32_t field_3BC;
	int32_t field_3C0;
	int32_t field_3C4;
	int32_t field_3C8;
	int32_t field_3CC;
	int32_t field_3D0;
	int32_t field_3D4;
	int32_t field_3D8;
	int32_t field_3DC;
	int32_t field_3E0;
	int32_t field_3E4;
	int32_t field_3E8;
	int32_t field_3EC;
	int32_t field_3F0;
	int32_t field_3F4;
	int32_t field_3F8;
	int32_t field_3FC;
	int32_t field_400;
	int32_t field_404;
	int32_t field_408;
	int32_t field_40C;
	int32_t field_410;
	int32_t field_414;
	int32_t field_418;
	int32_t field_41C;
	int32_t field_420;
	int32_t field_424;
	int32_t field_428;
	int32_t field_42C;
	int32_t field_430;
	int32_t field_434;
	int32_t field_438;
	int32_t field_43C;
	int32_t field_440;
	int32_t field_444;
	int32_t field_448;
	int32_t field_44C;
	int32_t field_450;
	int32_t field_454;
	int32_t field_458;
	int32_t field_45C;
	int32_t field_460;
	int32_t field_464;
	int32_t field_468;
	int32_t field_46C;
	int32_t field_470;
	int32_t field_474;
	int32_t field_478;
	int32_t field_47C;
	int32_t field_480;
	int32_t field_484;
	int32_t field_488;
	int32_t field_48C;
	int32_t field_490;
	int32_t field_494;
	int32_t field_498;
	int32_t field_49C;
	int32_t field_4A0;
	int32_t field_4A4;
	int32_t field_4A8;
	int32_t field_4AC;
	int32_t field_4B0;
	int32_t field_4B4;
	int32_t field_4B8;
	int32_t field_4BC;
	int32_t field_4C0;
	int32_t field_4C4;
	int32_t field_4C8;
	int32_t field_4CC;
	int32_t field_4D0;
	int32_t field_4D4;
	int32_t field_4D8;
	int32_t field_4DC;
	int32_t field_4E0;
	int32_t field_4E4;
	int32_t field_4E8;
	int32_t field_4EC;
	int32_t field_4F0;
	int32_t field_4F4;
	int32_t field_4F8;
	int32_t field_4FC;
	int32_t field_500;
	int32_t field_504;
	int32_t field_508;
	int32_t field_50C;
	int32_t field_510;
	int32_t field_514;
	int32_t field_518;
	int32_t field_51C;
	int32_t field_520;
	int32_t field_524;
	int32_t field_528;
	int32_t field_52C;
	int32_t field_530;
	int32_t field_534;
	int32_t field_538;
	int32_t field_53C;
	int32_t field_540;
	int32_t field_544;
	int32_t skin_equip;
	int32_t skin_equip_cmn;
	int32_t use_pv_skin_equip;
	int32_t btn_se_equip;
	int32_t btn_se_equip_cmn;
	int32_t use_pv_btn_se_equip;
	int32_t slide_se_equip;
	int32_t slide_se_equip_cmn;
	int32_t use_pv_slide_se_equip;
	int32_t chainslide_se_equip;
	int32_t chainslide_se_equip_cmn;
	int32_t use_pv_chainslide_se_equip;
	int32_t slidertouch_se_equip;
	int32_t slidertouch_se_equip_cmn;
	int32_t use_pv_slidertouch_se_equip;
	int32_t field_584;
	int* mylistA_begin;
	int* mylistA_end;
	int32_t field_598;
	int32_t field_59C;
	int* mylistB_begin;
	int* mylistB_end;
	int32_t field_5B0;
	int32_t field_5B4;
	int* mylistC_begin;
	int* mylistC_end;
	int32_t field_5C8;
	int32_t field_5CC;
	int32_t field_5D0;
	int32_t field_5D4;
	int32_t field_5D8;
	int32_t field_5DC;
	int32_t field_5E0;
	int32_t field_5E4;
	int32_t field_5E8;
	int32_t field_5EC;
	int32_t field_5F0;
	int32_t field_5F4;
	int32_t field_5F8;
	int32_t field_5FC;
	int32_t field_600;
	int32_t field_604;
	int32_t field_608;
	int32_t field_60C;
	int32_t field_610;
	int32_t field_614;
	int32_t field_618;
	int32_t field_61C;
	int32_t field_620;
	int32_t field_624;
	int32_t field_628;
	int32_t field_62C;
	int32_t field_630;
	int32_t field_634;
	int32_t field_638;
	int32_t field_63C;
	int32_t field_640;
	int32_t field_644;
	int32_t field_648;
	int32_t field_64C;
	int32_t field_650;
	int32_t field_654;
	int32_t field_658;
	int32_t field_65C;
	int32_t field_660;
	int32_t field_664;
	int32_t field_668;
	int32_t field_66C;
	int32_t field_670;
	int32_t field_674;
	int32_t field_678;
	int32_t field_67C;
	int32_t field_680;
	int32_t field_684;
	int32_t field_688;
	int32_t field_68C;
	int32_t field_690;
	int32_t field_694;
	int32_t field_698;
	int32_t field_69C;
	int32_t field_6A0;
	int32_t field_6A4;
	int32_t field_6A8;
	int32_t field_6AC;
	int32_t field_6B0;
	int32_t field_6B4;
	int32_t field_6B8;
	int32_t field_6BC;
	int32_t field_6C0;
	int32_t field_6C4;
	int32_t field_6C8;
	int32_t field_6CC;
	int32_t field_6D0;
	int32_t field_6D4;
	int32_t field_6D8;
	int32_t field_6DC;
	int32_t field_6E0;
	int32_t field_6E4;
	int32_t field_6E8;
	int32_t field_6EC;
	int32_t field_6F0;
	int32_t field_6F4;
	int32_t field_6F8;
	int32_t field_6FC;
	int32_t field_700;
	int32_t field_704;
	int32_t field_708;
	int32_t field_70C;
	int32_t field_710;
	int32_t field_714;
	int32_t field_718;
	int32_t field_71C;
	int32_t field_720;
	int32_t field_724;
	int32_t field_728;
	int32_t field_72C;
	int32_t field_730;
	int32_t field_734;
	int32_t field_738;
	int32_t field_73C;
	int32_t field_740;
	int32_t field_744;
	int32_t field_748;
	int32_t field_74C;
	int32_t field_750;
	int32_t field_754;
	int32_t field_758;
	int32_t field_75C;
	int32_t field_760;
	int32_t field_764;
	int32_t field_768;
	int32_t field_76C;
	int32_t field_770;
	int32_t field_774;
	int32_t field_778;
	int32_t field_77C;
	int32_t field_780;
	int32_t field_784;
	int32_t field_788;
	int32_t field_78C;
	int32_t field_790;
	int32_t field_794;
	int32_t field_798;
	int32_t field_79C;
	int32_t field_7A0;
	int32_t field_7A4;
	int32_t field_7A8;
	int32_t field_7AC;
	int32_t field_7B0;
	int32_t field_7B4;
	int32_t field_7B8;
	int32_t field_7BC;
	int32_t field_7C0;
	int32_t field_7C4;
	int32_t field_7C8;
	int32_t field_7CC;
	int32_t field_7D0;
	int32_t field_7D4;
	int32_t field_7D8;
	int32_t field_7DC;
	int32_t field_7E0;
	int32_t field_7E4;
	int32_t field_7E8;
	int32_t field_7EC;
	int32_t field_7F0;
	int32_t field_7F4;
	int32_t field_7F8;
	int32_t field_7FC;
	int32_t field_800;
	int32_t field_804;
	int32_t field_808;
	int32_t field_80C;
	int32_t field_810;
	int32_t field_814;
	int32_t field_818;
	int32_t field_81C;
	int32_t field_820;
	int32_t field_824;
	int32_t field_828;
	int32_t field_82C;
	int32_t field_830;
	int32_t field_834;
	int32_t field_838;
	int32_t field_83C;
	int32_t field_840;
	int32_t field_844;
	int32_t field_848;
	int32_t field_84C;
	int32_t field_850;
	int32_t field_854;
	int32_t field_858;
	int32_t field_85C;
	int32_t field_860;
	int32_t field_864;
	int32_t field_868;
	int32_t field_86C;
	int32_t field_870;
	int32_t field_874;
	int32_t field_878;
	int32_t field_87C;
	int32_t field_880;
	int32_t field_884;
	int32_t field_888;
	int32_t field_88C;
	int32_t field_890;
	int32_t field_894;
	int32_t field_898;
	int32_t field_89C;
	int32_t field_8A0;
	int32_t field_8A4;
	int32_t field_8A8;
	int32_t field_8AC;
	int32_t field_8B0;
	int32_t field_8B4;
	int32_t field_8B8;
	int32_t field_8BC;
	int32_t field_8C0;
	int32_t field_8C4;
	int32_t field_8C8;
	int32_t field_8CC;
	int32_t field_8D0;
	int32_t field_8D4;
	int32_t field_8D8;
	int32_t field_8DC;
	int32_t field_8E0;
	int32_t field_8E4;
	int32_t field_8E8;
	int32_t field_8EC;
	int32_t field_8F0;
	int32_t field_8F4;
	int32_t field_8F8;
	int32_t field_8FC;
	int32_t field_900;
	int32_t field_904;
	int32_t field_908;
	int32_t field_90C;
	int32_t field_910;
	int32_t field_914;
	int32_t field_918;
	int32_t field_91C;
	int32_t field_920;
	int32_t field_924;
	int32_t field_928;
	int32_t field_92C;
	int32_t field_930;
	int32_t field_934;
	int32_t field_938;
	int32_t field_93C;
	int32_t field_940;
	int32_t field_944;
	int32_t field_948;
	int32_t field_94C;
	int32_t field_950;
	int32_t field_954;
	int32_t field_958;
	int32_t field_95C;
	int32_t field_960;
	int32_t field_964;
	int32_t field_968;
	int32_t field_96C;
	int32_t field_970;
	int32_t field_974;
	int32_t field_978;
	int32_t field_97C;
	int32_t field_980;
	int32_t field_984;
	int32_t field_988;
	int32_t field_98C;
	int32_t field_990;
	int32_t field_994;
	int32_t field_998;
	int32_t field_99C;
	int32_t field_9A0;
	int32_t field_9A4;
	int32_t field_9A8;
	int32_t field_9AC;
	int32_t field_9B0;
	int32_t field_9B4;
	int32_t field_9B8;
	int32_t field_9BC;
	int32_t field_9C0;
	int32_t field_9C4;
	int32_t field_9C8;
	int32_t field_9CC;
	int32_t field_9D0;
	int32_t field_9D4;
	int32_t field_9D8;
	int32_t field_9DC;
	int32_t field_9E0;
	int32_t field_9E4;
	int32_t field_9E8;
	int32_t field_9EC;
	int32_t field_9F0;
	int32_t field_9F4;
	int32_t field_9F8;
	int32_t field_9FC;
	int32_t field_A00;
	int32_t field_A04;
	int32_t field_A08;
	int32_t field_A0C;
	int32_t field_A10;
	int32_t field_A14;
	int32_t field_A18;
	int32_t field_A1C;
	int32_t field_A20;
	int32_t field_A24;
	int32_t field_A28;
	int32_t field_A2C;
	int32_t field_A30;
	int32_t field_A34;
	int32_t field_A38;
	int32_t field_A3C;
	int32_t field_A40;
	int32_t field_A44;
	int32_t field_A48;
	int32_t field_A4C;
	int32_t field_A50;
	int32_t field_A54;
	int32_t field_A58;
	int32_t field_A5C;
	int32_t field_A60;
	int32_t field_A64;
	int32_t field_A68;
	int32_t field_A6C;
	int32_t field_A70;
	int32_t field_A74;
	int32_t field_A78;
	int32_t field_A7C;
	int32_t field_A80;
	int32_t field_A84;
	int32_t field_A88;
	int32_t field_A8C;
	int32_t field_A90;
	int32_t field_A94;
	int32_t field_A98;
	int32_t field_A9C;
	int32_t field_AA0;
	int32_t field_AA4;
	int32_t field_AA8;
	int32_t field_AAC;
	int32_t field_AB0;
	int32_t field_AB4;
	int32_t field_AB8;
	int32_t field_ABC;
	int32_t field_AC0;
	int32_t field_AC4;
	int32_t field_AC8;
	int32_t field_ACC;
	int32_t field_AD0;
	int32_t field_AD4;
	int32_t field_AD8;
	int32_t field_ADC;
	int32_t field_AE0;
	int32_t field_AE4;
	int32_t field_AE8;
	int32_t field_AEC;
	int32_t field_AF0;
	int32_t field_AF4;
	int32_t field_AF8;
	int32_t field_AFC;
	int32_t field_B00;
	int32_t field_B04;
	int32_t field_B08;
	int32_t field_B0C;
	int32_t field_B10;
	int32_t field_B14;
	int32_t field_B18;
	int32_t field_B1C;
	int32_t field_B20;
	int32_t field_B24;
	int32_t field_B28;
	int32_t field_B2C;
	int32_t field_B30;
	int32_t field_B34;
	int32_t field_B38;
	int32_t field_B3C;
	int32_t field_B40;
	int32_t field_B44;
	int32_t field_B48;
	int32_t field_B4C;
	int32_t field_B50;
	int32_t field_B54;
	int32_t field_B58;
	int32_t field_B5C;
	int32_t field_B60;
	int32_t field_B64;
	int32_t field_B68;
	int32_t field_B6C;
	int32_t field_B70;
	int32_t field_B74;
	int32_t field_B78;
	int32_t field_B7C;
	int32_t field_B80;
	int32_t field_B84;
	int32_t field_B88;
	int32_t field_B8C;
	int32_t field_B90;
	int32_t field_B94;
	int32_t field_B98;
	int32_t field_B9C;
	int32_t field_BA0;
	int32_t field_BA4;
	int32_t field_BA8;
	int32_t field_BAC;
	int32_t field_BB0;
	int32_t field_BB4;
	int32_t field_BB8;
	int32_t field_BBC;
	int32_t field_BC0;
	int32_t field_BC4;
	int32_t field_BC8;
	int32_t field_BCC;
	int32_t field_BD0;
	int32_t field_BD4;
	int32_t field_BD8;
	int32_t field_BDC;
	int32_t field_BE0;
	int32_t field_BE4;
	int32_t field_BE8;
	int32_t field_BEC;
	int32_t field_BF0;
	int32_t field_BF4;
	int32_t field_BF8;
	int32_t field_BFC;
	int32_t field_C00;
	int32_t field_C04;
	int32_t field_C08;
	int32_t field_C0C;
	int32_t field_C10;
	int32_t field_C14;
	int32_t field_C18;
	int32_t field_C1C;
	int32_t field_C20;
	int32_t field_C24;
	int32_t field_C28;
	int32_t field_C2C;
	int32_t field_C30;
	int32_t field_C34;
	int32_t field_C38;
	int32_t field_C3C;
	int32_t field_C40;
	int32_t field_C44;
	int32_t field_C48;
	int32_t field_C4C;
	int32_t field_C50;
	int32_t field_C54;
	int32_t field_C58;
	int32_t field_C5C;
	int32_t field_C60;
	int32_t field_C64;
	int32_t field_C68;
	int32_t field_C6C;
	int32_t field_C70;
	int32_t field_C74;
	int32_t field_C78;
	int32_t field_C7C;
	int32_t field_C80;
	int32_t field_C84;
	int32_t field_C88;
	int32_t field_C8C;
	int32_t field_C90;
	int32_t field_C94;
	int32_t field_C98;
	int32_t field_C9C;
	int32_t field_CA0;
	int32_t field_CA4;
	int32_t field_CA8;
	int32_t field_CAC;
	int32_t field_CB0;
	int32_t field_CB4;
	int32_t field_CB8;
	int32_t field_CBC;
	int32_t field_CC0;
	int32_t field_CC4;
	int32_t field_CC8;
	int32_t field_CCC;
	int32_t field_CD0;
	int32_t field_CD4;
	int32_t field_CD8;
	int32_t field_CDC;
	int32_t field_CE0;
	int32_t field_CE4;
	int32_t field_CE8;
	int32_t field_CEC;
	int32_t field_CF0;
	int32_t field_CF4;
	int32_t field_CF8;
	int32_t field_CFC;
	int32_t field_D00;
	int32_t field_D04;
	int32_t field_D08;
	int32_t field_D0C;
	int32_t field_D10;
	int32_t field_D14;
	int32_t field_D18;
	int32_t field_D1C;
	int32_t field_D20;
	int32_t field_D24;
	int32_t field_D28;
	int32_t field_D2C;
	int32_t field_D30;
	int32_t field_D34;
	int32_t field_D38;
	int32_t field_D3C;
	int32_t field_D40;
	int32_t field_D44;
	int32_t field_D48;
	int32_t field_D4C;
	int32_t field_D50;
	int32_t field_D54;
	int32_t field_D58;
	int32_t field_D5C;
	int32_t field_D60;
	int32_t field_D64;
	int32_t field_D68;
	int32_t field_D6C;
	int32_t field_D70;
	int32_t field_D74;
	int32_t field_D78;
	int32_t field_D7C;
	int32_t field_D80;
	int32_t field_D84;
	int32_t field_D88;
	int32_t field_D8C;
	int32_t field_D90;
	uint8_t clear_border;
	uint8_t field_D95;
	uint8_t field_D96;
	uint8_t field_D97;
	int32_t field_D98;
	int32_t field_D9C;
	int32_t field_DA0;
	int32_t field_DA4;
	int32_t field_DA8;
	int32_t field_DAC;
	int32_t field_DB0;
	int32_t field_DB4;
	int32_t field_DB8;
	int32_t field_DBC;
	int32_t field_DC0;
	int32_t field_DC4;
	int32_t field_DC8;
	int32_t field_DCC;
	int32_t field_DD0;
	int32_t field_DD4;
	int32_t field_DD8;
	int32_t field_DDC;
	int32_t field_DE0;
	int32_t field_DE4;
	int32_t field_DE8;
	int32_t field_DEC;
	int32_t field_DF0;
	int32_t field_DF4;
	int32_t field_DF8;
	int32_t field_DFC;
	int32_t field_E00;
	int32_t field_E04;
	int32_t field_E08;
	int32_t field_E0C;
	int32_t field_E10;
	int32_t field_E14;
	int32_t field_E18;
	int32_t field_E1C;
	int32_t field_E20;
	int32_t field_E24;
	int32_t field_E28;
	int32_t field_E2C;
	int32_t field_E30;
	int8_t field_E34;
	int8_t game_opts;
	int8_t field_E36;
	int8_t field_E37;
	int32_t field_E38;
	int32_t field_E3C;
	int32_t field_E40;
	int32_t field_E44;
	int32_t field_E48;
	int32_t field_E4C;
	int32_t field_E50;
	int32_t field_E54;
	int32_t field_E58;
	int32_t field_E5C;
	int32_t field_E60;
	int32_t field_E64;
	int32_t field_E68;
	int32_t field_E6C;
	int32_t field_E70;
	int32_t field_E74;
	int32_t field_E78;
	int32_t field_E7C;
	int32_t field_E80;
	int32_t field_E84;
	int32_t field_E88;
	int32_t field_E8C;
	int32_t field_E90;
	int32_t field_E94;
	int32_t field_E98;
	int32_t field_E9C;
	int32_t field_EA0;
	int32_t field_EA4;
	int32_t field_EA8;
	int32_t field_EAC;
	int32_t field_EB0;
	int32_t field_EB4;
	int32_t field_EB8;
	int32_t field_EBC;
	int32_t field_EC0;
	int32_t field_EC4;
	int32_t field_EC8;
	int32_t field_ECC;
	int32_t field_ED0;
	int32_t field_ED4;
	int32_t field_ED8;
	int32_t field_EDC;
	int32_t field_EE0;
	int32_t field_EE4;
	int32_t field_EE8;
	int32_t field_EEC;
	int32_t field_EF0;
	int32_t field_EF4;
	int32_t field_EF8;
	int32_t field_EFC;
};