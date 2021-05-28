class CViewSetup
{
public:
    uint16_t x; //0x0010
    uint16_t y; //0x0014
    uint16_t width; //0x0018
    uint16_t height; //0x001C
    /*
    char pad_0000[4]; //0x0000
    uint32_t N00000050; //0x0004
    uint32_t N00000051; //0x0008
    uint32_t N00000052; //0x000C
    uint32_t width; //0x0010
    uint32_t unscale_width; //0x0014
    uint32_t height; //0x0018
    uint32_t unscale_height; //0x001C
    bool N00000057; //0x0020
    char pad_0021[3]; //0x0021
    float N00000058; //0x0024
    float N00000059; //0x0028
    float N0000005A; //0x002C
    float N0000005B; //0x0030
    */
    //std::byte pad[176];
    std::byte pad[168];
    float fov;
    float m_viewmodelfov; //0x0038
    Vector origin;
    Vector angles;
    float zNear;
    float zFar;
    float zNearViewmodel;
    float zFarViewmodel;
    float m_flAspectRatio;
    float m_flNearBlurDepth;
    float m_flNearFocusDepth;
    float m_flFarFocusDepth;
    float m_flFarBlurDepth;
    float m_flNearBlurRadius;
    float m_flFarBlurRadius;
    float m_nDoFQuality;
    __int32 m_nMotionBlurMode;
    std::byte _0x0104[68];
    __int32 m_EdgeBlur;	
}; //Size: 0x0840


struct ViewSetup {
    std::byte pad[176];
    float fov;
    std::byte pad1[32];
    float farZ;
};