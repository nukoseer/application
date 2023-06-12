#include "types.h"
#include "utils.h"

#include "os_random.h"

#define MAX_RANDOM_NUMBER 0xFFFFFFFF

static const u32 os_random_number_sequence[] =
{
    0x4C11971, 0x4A67792, 0x1B87D07, 0x0E91547, 0x2D1FD27, 0x2926C0A, 0x5B9F75B, 0x410AAFC,
    0x168F4E6, 0x43ADE9F, 0x14BFA12, 0x02D36DF, 0x02B86B6, 0x133DAEC, 0x226031F, 0x4319A80,
    0x56243EC, 0x1A9F064, 0x35D2FE5, 0x41D2E79, 0x29FAE99, 0x247C1D5, 0x1F8A68A, 0x2229293,
    0x116944C, 0x514B1D6, 0x315A82D, 0x5D2442B, 0x0FE2D47, 0x2576CA4, 0x090AEF6, 0x240CD10,
    0x37CF60A, 0x2397E3D, 0x16C5738, 0x2239B54, 0x283CE1B, 0x42AE8C6, 0x2DF4DF0, 0x502109A,
    0x5CBD52F, 0x48FE38C, 0x49439C1, 0x4959857, 0x3BC54D7, 0x016774C, 0x4953132, 0x577EC3C,
    0x34C0B1E, 0x00F6429, 0x264B6BB, 0x47114D4, 0x54E97E9, 0x048B7CA, 0x47C66F9, 0x26D8540,
    0x0764615, 0x1C0FB5A, 0x160B8C3, 0x4309A19, 0x46F3A44, 0x34887EF, 0x1296F60, 0x380EC85,
    0x1AC742B, 0x0AD7771, 0x2BE4772, 0x54AB6C0, 0x3A22F38, 0x2C667A3, 0x29F61B1, 0x12AE2A9,
    0x3A6AB34, 0x2C68417, 0x02B390E, 0x2448B3E, 0x27A1FA4, 0x3C2676C, 0x5070384, 0x4DBE5A4,
    0x2D69FC4, 0x1724A58, 0x472C06F, 0x53189DA, 0x31A948C, 0x4DE6234, 0x4AAC2A0, 0x0C9DE19,
    0x378BC0E, 0x0ABF8F7, 0x4919BED, 0x3C4A716, 0x251752B, 0x31468FF, 0x22E6492, 0x43E946D,
    0x135BFA1, 0x322DF12, 0x5BACB1C, 0x5CF57E1, 0x5483F98, 0x1264ED3, 0x45063BF, 0x09FA86A,
    0x12C5465, 0x3F043B0, 0x3195E11, 0x5AC2870, 0x4753103, 0x516BF3A, 0x4FB2DE4, 0x298D0AD,
    0x1AB2882, 0x4918472, 0x39BF980, 0x0F74EEE, 0x3AF2CC7, 0x5052C00, 0x3F5E145, 0x21ADF54,
    0x3BF0C56, 0x30E5449, 0x39A79B4, 0x1C133D7, 0x1E11342, 0x4D23CBD, 0x5AC7520, 0x0627900,
    0x16F9EB7, 0x4E9A71D, 0x5EF5ED4, 0x0974C7F, 0x26A581D, 0x0276C23, 0x0DB562E, 0x46E0B64,
    0x388DB97, 0x36D09FF, 0x066C7B1, 0x2D1981E, 0x52FE130, 0x3245798, 0x13FBA6A, 0x5060150,
    0x0622C0C, 0x25CA431, 0x0520696, 0x5519602, 0x2250370, 0x4619B3A, 0x44D5CFA, 0x479507C,
    0x48F92E8, 0x1EC3946, 0x17DC888, 0x2B286FF, 0x0CDC15D, 0x10FE740, 0x0B9912E, 0x0F9B742,
    0x3F9FEC5, 0x0D6CDD0, 0x5EA31FD, 0x1A92207, 0x01BA394, 0x4FA21A2, 0x3F8245B, 0x0213A05,
    0x438127E, 0x45C66F5, 0x44AA2FA, 0x4D7E527, 0x4B0703C, 0x4AC13BC, 0x1F72E20, 0x555C24B,
    0x46C5CBB, 0x1E0B6DA, 0x4497832, 0x5387C42, 0x09453FB, 0x59571AA, 0x30DDD43, 0x1005338,
    0x45A15EB, 0x3459242, 0x4B209A5, 0x2F76930, 0x03E3019, 0x2146785, 0x0345E05, 0x06CD257,
    0x14C1532, 0x2D0AE2F, 0x01B23C9, 0x237ED89, 0x259B4E3, 0x588D7F9, 0x13EC3C5, 0x34E4FF6,
    0x3AD0D64, 0x2DF77E5, 0x0153454, 0x2103DDF, 0x357F488, 0x2140332, 0x34621D7, 0x4D85A75,
    0x14ABFE4, 0x468B006, 0x3A37F18, 0x2B2C51B, 0x18A1501, 0x4E05C31, 0x1581A8F, 0x1BCABCF,
    0x1735DBF, 0x2A64AFF, 0x087293B, 0x4B4BC6A, 0x21273B8, 0x53B9F01, 0x431BA84, 0x32E6A55,
    0x0949CE2, 0x1883F12, 0x53271DB, 0x4AF09AD, 0x0763525, 0x1C3F452, 0x4D617C4, 0x0B8F52A,
    0x1E67F6B, 0x14D7EE4, 0x1E38206, 0x2E74276, 0x21A608A, 0x40BC5ED, 0x00C43A7, 0x3A98750,
    0x1BAD59C, 0x1F69142, 0x28F1C48, 0x5021395, 0x09B3603, 0x5A010EC, 0x13D802C, 0x5BD4AF0,
    0x513563B, 0x41AACC9, 0x21ABF69, 0x3628D6F, 0x1E4A33D, 0x29A378D, 0x2ACE13E, 0x2771E09,
    0x54F270D, 0x272EFD2, 0x36C1068, 0x38AB95B, 0x4FD0798, 0x0BDE0C3, 0x0EF8AAD, 0x3B4DEF1,
    0x30D15EF, 0x00D4404, 0x0C0715E, 0x58F0A12, 0x102C224, 0x1873B88, 0x395B3F5, 0x5F4D760,
    0x046344F, 0x294D646, 0x3B4FCC8, 0x36133C1, 0x27593BB, 0x4DC7EEE, 0x23D77C4, 0x0013FEC,
    0x5086CBD, 0x305A6F4, 0x3D72351, 0x1597A82, 0x11C26BF, 0x2E0390A, 0x2FEC5D7, 0x5C3DC99,
    0x599286B, 0x3E25DDC, 0x53C1E49, 0x221B0CE, 0x3DBBF8E, 0x03F9EA0, 0x4CB3742, 0x41C1A65,
    0x4625147, 0x1AB648E, 0x438A31F, 0x0203627, 0x1CA3F10, 0x3712F2C, 0x256327F, 0x0F2DBEB,
    0x57AAB4C, 0x1181280, 0x046F77E, 0x155205F, 0x50B79A6, 0x548F5B4, 0x571B64D, 0x0FA6BC2,
    0x53CBA3B, 0x0360432, 0x42582A8, 0x03B7601, 0x1A5D115, 0x0A0E291, 0x1BD0CB5, 0x14E1EDC,
    0x49BCDD0, 0x3099E36, 0x1D103F7, 0x02C9016, 0x16922CA, 0x480AB90, 0x44924B1, 0x4CA48C4,
    0x55CD7E6, 0x189F4FC, 0x1A60C47, 0x0812F66, 0x5D5048B, 0x394E712, 0x32FCD0F, 0x51840D4,
    0x0BA5BE7, 0x36AA4B3, 0x2814C7A, 0x026A76C, 0x27867C6, 0x269AC0A, 0x09734FA, 0x3B51198,
    0x0526C30, 0x34E045D, 0x02D4276, 0x585A370, 0x21E1F7C, 0x17DAB51, 0x4AF8CD2, 0x5155081,
    0x25F7988, 0x001B246, 0x1664D5B, 0x4CC2304, 0x15EC1B0, 0x201B11F, 0x50DC2DE, 0x58544BE,
    0x29E7592, 0x3F72724, 0x01AC9FF, 0x1418793, 0x5C08C7A, 0x273181F, 0x0F031D3, 0x32C4687,
    0x40401A8, 0x45C519B, 0x4CC041A, 0x2CC62E8, 0x2ED1718, 0x16EAA88, 0x4E52B7D, 0x19EF450,
    0x474079C, 0x3D84922, 0x1A977DB, 0x5031ECF, 0x255D657, 0x5E60907, 0x399A2D7, 0x3084CA5,
    0x0697BAA, 0x3F95A51, 0x2FFCB81, 0x30ECCF8, 0x09F4F07, 0x383B467, 0x0195113, 0x1A1A07A,
    0x3D0DBE6, 0x0A4870A, 0x223DD9A, 0x0971100, 0x30E9658, 0x0720EFC, 0x2721930, 0x4612C76,
    0x294AF56, 0x25BF2AC, 0x1667BC8, 0x5F20E88, 0x5902493, 0x0C27494, 0x1700D64, 0x1BAAA27,
    0x2CA7E1E, 0x243ED11, 0x5C40CB4, 0x0762568, 0x103FF18, 0x103D2C4, 0x3BEA10A, 0x0B2F39F,
    0x094C2AF, 0x3CD2119, 0x594C86E, 0x2BCC45A, 0x382EDD5, 0x024116C, 0x27E6E87, 0x1F62792,
    0x4504C8A, 0x588B2DD, 0x0213AEE, 0x2946376, 0x29E4163, 0x14DB70B, 0x5680364, 0x5165CA3,
    0x3DE016B, 0x2AA8EFB, 0x1DD3B94, 0x42113C7, 0x098FAB5, 0x4D74EA5, 0x008DD59, 0x2018FDC,
    0x28E0B9C, 0x0B10B8C, 0x09DDEF0, 0x298F2C5, 0x311FBEC, 0x0758A1A, 0x41B02BE, 0x53ED1DE,
    0x3130D6A, 0x0AFBACF, 0x51672BA, 0x55CBB3B, 0x3B3B89E, 0x1A8B2C6, 0x09B7FC0, 0x114526E,
    0x5C21791, 0x07958FC, 0x10B01DA, 0x395BC16, 0x25192DB, 0x3FEE261, 0x0933709, 0x26F5FC5,
    0x0637B79, 0x16A6E95, 0x5DF02DF, 0x228FB04, 0x44448CB, 0x0F063D5, 0x4068902, 0x02CF9C3,
    0x0FA15FD, 0x5E01C46, 0x25505EE, 0x069B845, 0x4A86BC1, 0x342863E, 0x451F4C0, 0x4895FAB,
    0x1552788, 0x0E7F838, 0x5B62150, 0x16D1550, 0x1599CB9, 0x23A2C8D, 0x2513092, 0x4E6AB2A,
    0x5E011EB, 0x2E60A6A, 0x558E6D3, 0x012014D, 0x3E5F010, 0x5DBD515, 0x362C960, 0x443275B,
    0x3BB4FAB, 0x0511E0E, 0x03206D0, 0x397E906, 0x4278F77, 0x294AD03, 0x4F80CF6, 0x0481A05,
    0x122E322, 0x418BB1C, 0x25AECD8, 0x18133AE, 0x0B26786, 0x276FFB7, 0x33F910A, 0x0C74863,
    0x0F6A63B, 0x29315C6, 0x1F88D26, 0x5AF7204, 0x220D0DF, 0x2293199, 0x44CB225, 0x301DA4E,
    0x1E4EBB7, 0x215A965, 0x0635C0C, 0x27ADD16, 0x3C0D08E, 0x2A01140, 0x19CD9AC, 0x292B400,
    0x4F3C89B, 0x17410DA, 0x531305E, 0x42CE0BA, 0x29032D4, 0x26C7BF1, 0x256B8CE, 0x0881934,
    0x451CE66, 0x4078D06, 0x1159E34, 0x5A0ED28, 0x12F618E, 0x3A36F04, 0x372B9A8, 0x51C3C40,
    0x074E0E4, 0x2FF84BB, 0x4ABB95C, 0x33F6F28, 0x5EBE462, 0x5421362, 0x4F4EA13, 0x0ACD489,
    0x0EB63E7, 0x0D5CEB9, 0x43CC127, 0x1015AEC, 0x4114C6E, 0x4DC7D50, 0x06D46D0, 0x33C3AA8,
    0x245F489, 0x056D0EB, 0x5D75302, 0x1B96D3E, 0x5C2E441, 0x55F3EF8, 0x4D728E9, 0x581561B,
    0x316B688, 0x470C1D3, 0x4CEA64F, 0x36E0820, 0x1BC673E, 0x4432C7D, 0x1BEBD53, 0x445F217,
    0x16ECFCF, 0x48460A7, 0x52204CA, 0x1562AB6, 0x166B6EA, 0x33FE97D, 0x137979A, 0x327755B,
    0x00EF8A3, 0x280F053, 0x559907C, 0x39B2FA4, 0x2592FA6, 0x23B1FFA, 0x05C6909, 0x4EC1B6A,
    0x1BEA4A0, 0x4896E66, 0x266531A, 0x4781265, 0x1B0B022, 0x06DE578, 0x04AC2F0, 0x49A2E1E,
    0x38A58CF, 0x1EA5789, 0x500235A, 0x1123C49, 0x1162410, 0x3C243D1, 0x4141CC5, 0x149A37F,
    0x4AFACC0, 0x52A2C67, 0x5E7A24B, 0x0326D6C, 0x39A8BEA, 0x1CC319C, 0x23D2783, 0x37F8665,
    0x589CFE3, 0x49457DE, 0x19B8F0F, 0x4C23B80, 0x497FE56, 0x0F2F479, 0x0C4C385, 0x571790F,
    0x4C0E51E, 0x54D0923, 0x4842EC5, 0x0B61C2A, 0x46AF43C, 0x03A7E03, 0x3F52A4E, 0x02A9AE2,
    0x1608BCE, 0x39ED023, 0x1EDDAFD, 0x095A9C1, 0x5C44717, 0x5C7A3AD, 0x3E3E01D, 0x4392C37,
    0x4FE58F9, 0x127CABF, 0x0E295A8, 0x2FFAACD, 0x1738C62, 0x3C29F59, 0x2AC7AA3, 0x4AD08EC,
    0x13FBCAC, 0x532AE2D, 0x477D9AC, 0x2315B2E, 0x5466345, 0x233B7D3, 0x2BC468C, 0x4740231,
    0x1F8072E, 0x311FE1E, 0x4F85ABD, 0x14A349C, 0x5034F81, 0x24C1D7D, 0x2D0C1F4, 0x33F0D20,
    0x4EF1A2A, 0x4E6DF89, 0x44ECBC4, 0x2CB5D1E, 0x1F81D1A, 0x07B9A9A, 0x039354A, 0x5715F1F,
    0x518A742, 0x2ECC97F, 0x4ADD990, 0x2F2624C, 0x0470242, 0x0FEBA60, 0x2CCD7C9, 0x3FB18FC,
    0x38EDC28, 0x0843AD1, 0x500AB5D, 0x5458947, 0x3886AC9, 0x221DC96, 0x217EFD0, 0x3373D77,
    0x4F86E96, 0x374A76A, 0x15366A1, 0x060C15A, 0x51E33D4, 0x288547E, 0x4D24A1A, 0x29057D1,
    0x4794034, 0x45899C7, 0x506B8F3, 0x0D5456C, 0x2BD2816, 0x1CF7FAD, 0x027641F, 0x319EAC9,
    0x5792C10, 0x27D155C, 0x15D1BF1, 0x1E4823A, 0x47F838C, 0x3C858C9, 0x564158A, 0x4D30AAB,
    0x284BC97, 0x22DB525, 0x29CBE58, 0x5D1344F, 0x3D37D2E, 0x12EC09C, 0x0D31822, 0x5BF80D9,
    0x2597E8B, 0x498A983, 0x2F1240C, 0x367551F, 0x08588D7, 0x212D55A, 0x3DDAB70, 0x5BBA638,
    0x1B1A80E, 0x1A7338A, 0x3CDB448, 0x3AB0EBA, 0x109EC1D, 0x2626250, 0x23ABB88, 0x13A0167,
    0x587118E, 0x425EF4C, 0x32F551F, 0x2226F9B, 0x1CAF625, 0x53D60BE, 0x0DC63C1, 0x58D8947,
    0x2C6DB15, 0x55AD522, 0x5A0A30F, 0x3524DBC, 0x17F55BA, 0x52A8782, 0x1F4FF80, 0x20F42B7,
    0x4756997, 0x2D9D4D1, 0x5A6FABE, 0x14CD18B, 0x48F984D, 0x090B228, 0x3700B57, 0x0812637,
    0x5613533, 0x475C91E, 0x2D5D590, 0x0732F83, 0x079A0FF, 0x492C84F, 0x4C8E270, 0x21A0D86,
    0x1B21B61, 0x5646D14, 0x0515810, 0x17EE2FE, 0x1D509B2, 0x0C5782E, 0x53CD2AC, 0x3AC7627,
    0x3C17112, 0x470D7F8, 0x2AA8B1D, 0x013C96A, 0x4AF496C, 0x00180B3, 0x1FFB52D, 0x5CC0A17,
    0x093FA22, 0x1B13850, 0x5B1B912, 0x176C4FB, 0x4FB9B4A, 0x3D480C0, 0x4D6991E, 0x24B3136,
    0x403FE4C, 0x39B4AFF, 0x4D66F5E, 0x3DDE16B, 0x599E992, 0x2F2B760, 0x36E845C, 0x55A83D8,
    0x5A61DEC, 0x050C2F9, 0x50A7AAA, 0x3405FBE, 0x5E6D6DE, 0x436D066, 0x3E272E1, 0x5A7057E,
    0x5C850FD, 0x5660FD2, 0x13272A1, 0x5EF2684, 0x329A8FC, 0x2C73666, 0x3919328, 0x26AC100,
    0x4BA72B1, 0x58EEE96, 0x5829E8F, 0x4D72757, 0x567F15E, 0x25C7F1F, 0x51F4182, 0x430AA86,
    0x0A23389, 0x455659E, 0x1778EB4, 0x373E1CD, 0x4E652F3, 0x2952315, 0x585B23E, 0x2288C4A,
    0x182E040, 0x5A5E833, 0x4545C65, 0x4CCA8E5, 0x3BD0F55, 0x4AC1BEC, 0x466DE2F, 0x1B1F9E6,
    0x5466E01, 0x412BE99, 0x32AFBC2, 0x07610DD, 0x0001CC8, 0x3DE6125, 0x0F20C7D, 0x2EB20EC,
    0x17E70F3, 0x1B8A122, 0x3A16B8E, 0x5D3E638, 0x06FB7AA, 0x5070B7F, 0x34EDAED, 0x1E411C1,
    0x271092C, 0x27A9220, 0x519B8DC, 0x4E41114, 0x046A8A2, 0x309F5EA, 0x412C325, 0x5435100,
    0x1A767B5, 0x17739F4, 0x45E2BC4, 0x444C48C, 0x56FC7A0, 0x3D5620E, 0x44DBDEF, 0x29575A2,
    0x559EE4F, 0x3951CE5, 0x51B959E, 0x23A0B33, 0x584B96F, 0x2ED10F1, 0x0AAD009, 0x26D460D,
    0x49C89FF, 0x3090376, 0x47249AF, 0x5BE8832, 0x2758E93, 0x5B9B0EC, 0x0450EF2, 0x40AFF1A,
    0x2AF2F50, 0x57EF01D, 0x4D244D3, 0x14E481D, 0x3F416CF, 0x4013676, 0x411BD66, 0x30638FE,
    0x3197756, 0x124FF5C, 0x1511AE6, 0x0D90D3F, 0x02C693E, 0x0909BFF, 0x37F19C7, 0x3003980,
    0x1299859, 0x2A5EA63, 0x2285815, 0x2E7A474, 0x559FBDC, 0x4AF67DD, 0x2FCD412, 0x4CEB3DC,
    0x2C874D9, 0x54E598E, 0x297405B, 0x3667FEA, 0x3044F7F, 0x5A1CA1D, 0x2EE506A, 0x4EB2DA4,
    0x139CBEC, 0x0B00F51, 0x1BF5C21, 0x36F7D25, 0x1718BDE, 0x1E98CAB, 0x239FC29, 0x5330ED3,
    0x21AB1CC, 0x2A09441, 0x5941C60, 0x43BFF95, 0x0F6E0BC, 0x4B5DF1E, 0x3D62519, 0x5513CB4,
    0x2768523, 0x26E6702, 0x1FA3BC4, 0x447AA73, 0x021326B, 0x46AC2CF, 0x046DF9B, 0x4F6C2C7,
    0x02BEF05, 0x35FF64E, 0x18F13C9, 0x1DE63DF, 0x23EF0C3, 0x0540C78, 0x2FC78CE, 0x37066D4,
    0x4ECD7EA, 0x2CC326D, 0x24F571B, 0x5A5F279, 0x1FA5FEB, 0x1FB3002, 0x285063F, 0x5D2BC0C,
    0x46DBBD0, 0x0BF82F3, 0x3BEDEA0, 0x147C8E1, 0x2AB1F5B, 0x5B8B9F4, 0x017CD49, 0x55B8E12,
    0x5A5892E, 0x4AC58E8, 0x2B9F966, 0x3E61271, 0x0E35372, 0x00CA36F, 0x383A762, 0x136E6AD,
    0x144DF3D, 0x2F52070, 0x5D92280, 0x12D4D6C, 0x442BDF3, 0x175B514, 0x5B814BF, 0x1A04132,
    0x23F7F81, 0x47572A1, 0x0A393EA, 0x05EC5BF, 0x2A82B4D, 0x447D304, 0x4BEDDCE, 0x48CE53E,
    0x4E7B483, 0x1CC6791, 0x36D4787, 0x453F7EB, 0x214A8F7, 0x2AE6F49, 0x554942D, 0x26AC6E0,
    0x0ED7283, 0x4C676AD, 0x12F6520, 0x1F3ADCF, 0x056B4F4, 0x0D5FC6D, 0x12021F8, 0x3FB5B1D,
    0x1B99046, 0x35FC0DC, 0x4278C0E, 0x4EB97D8, 0x18091A3, 0x40E629D, 0x15D2482, 0x505D51A,
    0x27FBC90, 0x0492A0D, 0x45CB10F, 0x1E1EF84, 0x2955F8D, 0x50E959B, 0x11B75C7, 0x53B5E1C,
};

static u32 next_random_seed(OSRandomHandle os_random_handle)
{
    u32 seed = os_random_number_sequence[os_random_handle.index++];

    if (os_random_handle.index >= ARRAY_COUNT(os_random_number_sequence))
    {
        os_random_handle.index = 0;
    }

    return seed;
}

static u32 xorwow_random(u32 seed)
{
    static u32 y = 362436069;
    static u32 z = 521288629;
    static u32 w = 88675123;
    static u32 v = 5783321;
    static u32 d = 6615241;
    u32 x = seed;
    u32 t = (x ^ (x >> 2));

    x = y;
    y = z;
    z = w;
    w = v;
    v = (v ^ (v << 4)) ^ (t ^ (t << 1));
    d += 362437;

    return d + v;
}
// 7    F    8    0    0    0    0    0
// 0111 1111 1000 0000 0000 0000 0000 0000
// TODO: Find the correct way to calculate random f32.
f32 os_random_unilateral(OSRandomHandle os_random_handle)
{
    u32 mantissa_mask = 0x3F800000U;
    f32 result = (f32)(mantissa_mask | (xorwow_random(next_random_seed(os_random_handle)) & 0x3FFFFFFFU));

    return result - 1.0f;
}

u32 os_random_next_u32(OSRandomHandle os_random_handle)
{
    u32 result = xorwow_random(next_random_seed(os_random_handle));

    return result;
}

OSRandomHandle os_random_init(u32 value)
{
    OSRandomHandle os_random_handle = { 0 };

    os_random_handle.index = value % ARRAY_COUNT(os_random_number_sequence);

    return os_random_handle;
}
