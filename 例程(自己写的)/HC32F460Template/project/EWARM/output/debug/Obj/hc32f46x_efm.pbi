      L7�        778�      88I	�    I	I:J	�    J	J:K	�    K	K7L	�    L	L7N	�    N	N:Q	�    Q	Q:T	�    T	dg	�	   	 g	il	�
   	
 l	tw	�   
 w	~�	�    �	� �	�    �	��	�    �	��	�    �	���K	 ���� ����L	 ����J	 ���� ����L	 ��!��L	 ��!��T	 ��2�� ����	g		 ��*�)
�) ��3�� ����
l		
 ��/�� ���4
�4 ��>�"�# �����	 ��+�
� ��(�� ���,
�, ��6�&�' ����w	
 ��,�0
�0 ��:�$�% ���(
(�(  ��-�)�"* ���5�N	 �5�I��  ���0
!�0 ��8�E
�E! �/�O�+�#, ���3�N	 �3�G�'
!�' ��/�
-�$ ��$���	 ��,�.�%/ ���3�(4 �����	 ��,�0�&1 ���'
5�') ��.�8�+9 ����Q	 ��'�7
2�7' ��>�6�*7 ���:�,; ����Q	 ��'�
<�-= ��$���	 ��,�,
>�,.= �&�1�e<�-= ����Q	 ��&�<
?�</ �3�D�@�0A �����	 ��,�B�1C �����	 ��,�D�2E �����	 ��/�F�3G ���H�4I ��   J "+6?H]n�����������������������������������������������������	�	�	�	�	�	�	�	�
�
�
�
�
�
hc32f46x_efm.h hc32f46x_utility.h EFM_LOCK EFM_UNLOCK EFM_KEY1 EFM_KEY2 EFM_PROTECT_ADDR_MSK IS_VALID_POINTER IS_VALID_FLASH_LATENCY IS_VALID_READ_MD IS_VALID_ERASE_PGM_MD IS_VALID_FLASH_FLAG IS_VALID_CLEAR_FLASH_FLAG IS_VALID_EFM_INT_SEL IS_VALID_FLASH_ADDR IS_VALID_OTP_LOCK_ADDR EFM_Unlock void EFM_Unlock(void) EFM_Lock void EFM_Lock(void) EFM_FlashCmd void EFM_FlashCmd(int) enNewState int EFM_SetLatency void EFM_SetLatency(int) u32Latency EFM_InstructionCacheCmd void EFM_InstructionCacheCmd(int) EFM_DataCacheRstCmd void EFM_DataCacheRstCmd(int) EFM_SetReadMode void EFM_SetReadMode(int) enReadMD EFM_ErasePgmCmd void EFM_ErasePgmCmd(int) EFM_SetErasePgmMode void EFM_SetErasePgmMode(int) EFM_InterruptCmd void EFM_InterruptCmd(int, int) enInt EFM_GetFlagStatus int EFM_GetFlagStatus(int) EFM_ClearFlag void EFM_ClearFlag(int) u32flag EFM_GetStatus int EFM_GetStatus(void) EFM_SetWinProtectAddr void EFM_SetWinProtectAddr(int) stcAddr EFM_SetBusState void EFM_SetBusState(int) enState EFM_SingleProgram int EFM_SingleProgram(int, int) EFM_SingleProgramRB int EFM_SingleProgramRB(int, int) EFM_Memcpy void * EFM_Memcpy(void *, void *, int) pvDst void * pvSrc u32Count EFM_SequenceProgram int EFM_SequenceProgram(int, int, void *) EFM_SectorErase int EFM_SectorErase(int) EFM_MassErase int EFM_MassErase(int) EFM_OtpLock int EFM_OtpLock(int) EFM_ReadUID int EFM_ReadUID(void)    5 &Mr���������������������������	�	�	�
�
�
����������������� c:hc32f46x_efm.c@3457@macro@EFM_LOCK c:hc32f46x_efm.c@3515@macro@EFM_UNLOCK c:hc32f46x_efm.c@3573@macro@EFM_KEY1 c:hc32f46x_efm.c@3628@macro@EFM_KEY2 c:hc32f46x_efm.c@3684@macro@EFM_PROTECT_ADDR_MSK c:hc32f46x_efm.c@3788@macro@IS_VALID_POINTER c:hc32f46x_efm.c@3898@macro@IS_VALID_FLASH_LATENCY c:hc32f46x_efm.c@5271@macro@IS_VALID_READ_MD c:hc32f46x_efm.c@5520@macro@IS_VALID_ERASE_PGM_MD c:hc32f46x_efm.c@6242@macro@IS_VALID_FLASH_FLAG c:hc32f46x_efm.c@6894@macro@IS_VALID_CLEAR_FLASH_FLAG c:hc32f46x_efm.c@7466@macro@IS_VALID_EFM_INT_SEL c:hc32f46x_efm.c@7788@macro@IS_VALID_FLASH_ADDR c:hc32f46x_efm.c@8113@macro@IS_VALID_OTP_LOCK_ADDR c:@F@EFM_Unlock c:@F@EFM_Lock c:@F@EFM_FlashCmd c:hc32f46x_efm.c@10363@F@EFM_FlashCmd@enNewState c:@F@EFM_SetLatency c:hc32f46x_efm.c@11879@F@EFM_SetLatency@u32Latency c:@F@EFM_InstructionCacheCmd c:hc32f46x_efm.c@12530@F@EFM_InstructionCacheCmd@enNewState c:@F@EFM_DataCacheRstCmd c:hc32f46x_efm.c@13160@F@EFM_DataCacheRstCmd@enNewState c:@F@EFM_SetReadMode c:hc32f46x_efm.c@13747@F@EFM_SetReadMode@enReadMD c:@F@EFM_ErasePgmCmd c:hc32f46x_efm.c@14350@F@EFM_ErasePgmCmd@enNewState c:@F@EFM_SetErasePgmMode c:hc32f46x_efm.c@15366@F@EFM_SetErasePgmMode@enReadMD c:@F@EFM_InterruptCmd c:hc32f46x_efm.c@16279@F@EFM_InterruptCmd@enInt c:hc32f46x_efm.c@16303@F@EFM_InterruptCmd@enNewState c:@F@EFM_GetFlagStatus c:@F@EFM_ClearFlag c:hc32f46x_efm.c@18632@F@EFM_ClearFlag@u32flag c:@F@EFM_GetStatus c:@F@EFM_SetWinProtectAddr c:hc32f46x_efm.c@20343@F@EFM_SetWinProtectAddr@stcAddr c:@F@EFM_SetBusState c:hc32f46x_efm.c@21027@F@EFM_SetBusState@enState c:@F@EFM_SingleProgram c:@F@EFM_SingleProgramRB c:hc32f46x_efm.c@F@EFM_Memcpy c:hc32f46x_efm.c@24195@F@EFM_Memcpy@pvDst c:hc32f46x_efm.c@24208@F@EFM_Memcpy@pvSrc c:hc32f46x_efm.c@24221@F@EFM_Memcpy@u32Count c:@F@EFM_SequenceProgram c:@F@EFM_SectorErase c:@F@EFM_MassErase c:@F@EFM_OtpLock c:@F@EFM_ReadUID     �<invalid loc> H:\ZLIOTA_BC260Y\1-基本控制篇\2-例程源码\1-硬件使用说明\HC32F460Template\driver\src\hc32f46x_efm.c 