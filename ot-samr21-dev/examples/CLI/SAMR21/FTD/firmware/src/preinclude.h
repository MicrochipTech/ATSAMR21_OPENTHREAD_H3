/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _PREINCLUDE_H    /* Guard against multiple inclusion */
#define _PREINCLUDE_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    /* ************************************************************************** */
    /** Descriptive Constant Name

      @Summary
        Brief one-line summary of the constant.
    
      @Description
        Full description, explaining the purpose and usage of the constant.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
      @Remarks
        Any additional remarks
     */
#define MBEDTLS_CONFIG_FILE                                          "openthread-mbedtls-config.h"
#define MBEDTLS_USER_CONFIG_FILE                                     "samr21-mbedtls-config.h"
#define NDEBUG
#define OPENTHREAD_CONFIG_ASSERT_ENABLE                              (1)
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE      (0)
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE   (0)
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS                     (1)
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT          (1)
#define OPENTHREAD_CONFIG_FILE                                       "openthread-core-samr21-config.h"
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE                           (1)
#define OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE                   (0)
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE                            (1)
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE                         (1)
#define OPENTHREAD_CONFIG_THREAD_VERSION                             (OT_THREAD_VERSION_1_2)
#define OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE                   "openthread-core-samr21-config-check.h"
//#define OPENTHREAD_PROJECT_CORE_CONFIG_FILE                          "openthread-core-samr21-config.h"
#define OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE           (1)
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT           (0)
#define OPENTHREAD_CONFIG_COAP_API_ENABLE                            (1)
#define OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE                    (0)
#define OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE             (0)
#define OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE                     (0)
//#define OT_PLATFORM                                                  (external)
#define PACKAGE_NAME                                                 "OPENTHREAD"
#define PACKAGE_VERSION                                              "OT_SAMR21"

#define SAMR21_XPLAINED_PRO 1
#define BOARD SAMR21_XPLAINED_PRO
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _PREINCLUDE_H */

/* *****************************************************************************
 End of File
 */
