# Microsoft Developer Studio Project File - Name="libbmdasn1_common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libbmdasn1_common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbmdasn1_common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdasn1_common.mak" CFG="libbmdasn1_common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdasn1_common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libbmdasn1_common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdasn1_common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libbmdasn1_common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../exe_trunk"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /I "../../../../../common/win32/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libbmdasn1_common - Win32 Release"
# Name "libbmdasn1_common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AAControls.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AcceptablePolicySet.c
# End Source File
# Begin Source File

SOURCE=..\AccessDescription.c
# End Source File
# Begin Source File

SOURCE=..\ACClearAttrs.c
# End Source File
# Begin Source File

SOURCE=..\Accuracy.c
# End Source File
# Begin Source File

SOURCE=..\AdministrationDomainName.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AlgAndLength.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AlgorithmConstraints.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AlgorithmConstraintSet.c
# End Source File
# Begin Source File

SOURCE=..\AlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\AnotherName.c
# End Source File
# Begin Source File

SOURCE=..\AttCertIssuer.c
# End Source File
# Begin Source File

SOURCE=..\AttCertValidityPeriod.c
# End Source File
# Begin Source File

SOURCE=..\AttCertVersion.c
# End Source File
# Begin Source File

SOURCE=..\Attribute.c
# End Source File
# Begin Source File

SOURCE=..\AttributeCertificate.c
# End Source File
# Begin Source File

SOURCE=..\AttributeCertificateInfo.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AttributeConstraints.c
# End Source File
# Begin Source File

SOURCE=..\AttributesSequence.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AttributeTrustCondition.c
# End Source File
# Begin Source File

SOURCE=..\AttributeType.c
# End Source File
# Begin Source File

SOURCE=..\AttributeTypeAndValue.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AttributeTypeConstraints.c
# End Source File
# Begin Source File

SOURCE=..\AttributeValue.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\AttributeValueConstraints.c
# End Source File
# Begin Source File

SOURCE=..\AttrSpec.c
# End Source File
# Begin Source File

SOURCE=..\AuthAttributes.c
# End Source File
# Begin Source File

SOURCE=..\AuthenticatedData.c
# End Source File
# Begin Source File

SOURCE=..\AuthorityInfoAccessSyntax.c
# End Source File
# Begin Source File

SOURCE=..\AuthorityKeyIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\BaseCRLNumber.c
# End Source File
# Begin Source File

SOURCE=..\BaseDistance.c
# End Source File
# Begin Source File

SOURCE=..\BasicConstraints.c
# End Source File
# Begin Source File

SOURCE=..\BMDGroup.c
# End Source File
# Begin Source File

SOURCE=..\bmdgroup_utils.c
# End Source File
# Begin Source File

SOURCE=..\BMDGroupPair.c
# End Source File
# Begin Source File

SOURCE=..\BMDGroupPairList.c
# End Source File
# Begin Source File

SOURCE=..\BMDGroups.c
# End Source File
# Begin Source File

SOURCE=..\BMDkeyIV.c
# End Source File
# Begin Source File

SOURCE=..\BuiltInDomainDefinedAttribute.c
# End Source File
# Begin Source File

SOURCE=..\BuiltInDomainDefinedAttributes.c
# End Source File
# Begin Source File

SOURCE=..\BuiltInStandardAttributes.c
# End Source File
# Begin Source File

SOURCE=..\Certificate.c
# End Source File
# Begin Source File

SOURCE=..\CertificateChoices.c
# End Source File
# Begin Source File

SOURCE=..\CertificateIssuer.c
# End Source File
# Begin Source File

SOURCE=..\CertificateList.c
# End Source File
# Begin Source File

SOURCE=..\CertificatePolicies.c
# End Source File
# Begin Source File

SOURCE=..\CertificateSerialNumber.c
# End Source File
# Begin Source File

SOURCE=..\CertificateSet.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CertificateTrustPoint.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CertificateTrustTrees.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CertInfoReq.c
# End Source File
# Begin Source File

SOURCE=..\CertPolicyId.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CertRefReq.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CertRevReq.c
# End Source File
# Begin Source File

SOURCE=..\ClassList.c
# End Source File
# Begin Source File

SOURCE=..\Clearance.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CMSAttrs.c
# End Source File
# Begin Source File

SOURCE=..\CMSVersion.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CommitmentRule.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CommitmentRules.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CommitmentType.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CommitmentTypeIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\CommonName.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\CommonRules.c
# End Source File
# Begin Source File

SOURCE=..\CommonUtils.c
# End Source File
# Begin Source File

SOURCE=..\ContentEncryptionAlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\ContentInfo.c
# End Source File
# Begin Source File

SOURCE=..\ContentType.c
# End Source File
# Begin Source File

SOURCE=..\Countersignature.c
# End Source File
# Begin Source File

SOURCE=..\CountryName.c
# End Source File
# Begin Source File

SOURCE=..\CPSuri.c
# End Source File
# Begin Source File

SOURCE=..\CRLDistributionPoints.c
# End Source File
# Begin Source File

SOURCE=..\CRLNumber.c
# End Source File
# Begin Source File

SOURCE=..\CRLReason.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\DeltaTime.c
# End Source File
# Begin Source File

SOURCE=..\Digest.c
# End Source File
# Begin Source File

SOURCE=..\DigestAlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\DigestAlgorithmIdentifiers.c
# End Source File
# Begin Source File

SOURCE=..\DigestedData.c
# End Source File
# Begin Source File

SOURCE=..\DigestInfo.c
# End Source File
# Begin Source File

SOURCE=..\DirectoryString.c
# End Source File
# Begin Source File

SOURCE=..\DisplayText.c
# End Source File
# Begin Source File

SOURCE=..\DistinguishedName.c
# End Source File
# Begin Source File

SOURCE=..\DistributionPoint.c
# End Source File
# Begin Source File

SOURCE=..\DistributionPointName.c
# End Source File
# Begin Source File

SOURCE=..\DomainComponent.c
# End Source File
# Begin Source File

SOURCE="..\Dss-Parms.c"
# End Source File
# Begin Source File

SOURCE=..\EDIPartyName.c
# End Source File
# Begin Source File

SOURCE=..\EmailAddress.c
# End Source File
# Begin Source File

SOURCE=..\EncapsulatedContentInfo.c
# End Source File
# Begin Source File

SOURCE=..\EncryptedContent.c
# End Source File
# Begin Source File

SOURCE=..\EncryptedContentInfo.c
# End Source File
# Begin Source File

SOURCE=..\EncryptedData.c
# End Source File
# Begin Source File

SOURCE=..\EncryptedKey.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\EnuRevReq.c
# End Source File
# Begin Source File

SOURCE=..\EnvelopedData.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\ESSCertID.c
# End Source File
# Begin Source File

SOURCE=..\ExtendedCertificate.c
# End Source File
# Begin Source File

SOURCE=..\ExtendedCertificateInfo.c
# End Source File
# Begin Source File

SOURCE=..\ExtendedCertificateOrCertificate.c
# End Source File
# Begin Source File

SOURCE=..\ExtendedNetworkAddress.c
# End Source File
# Begin Source File

SOURCE=..\Extension.c
# End Source File
# Begin Source File

SOURCE=..\ExtensionAttribute.c
# End Source File
# Begin Source File

SOURCE=..\ExtensionAttributes.c
# End Source File
# Begin Source File

SOURCE=..\ExtensionPhysicalDeliveryAddressComponents.c
# End Source File
# Begin Source File

SOURCE=..\Extensions.c
# End Source File
# Begin Source File

SOURCE=..\ExtKeyUsageSyntax.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\FieldOfApplication.c
# End Source File
# Begin Source File

SOURCE=..\FreshestCRL.c
# End Source File
# Begin Source File

SOURCE=..\GeneralName.c
# End Source File
# Begin Source File

SOURCE=..\GeneralNames.c
# End Source File
# Begin Source File

SOURCE=..\GeneralSubtree.c
# End Source File
# Begin Source File

SOURCE=..\GeneralSubtrees.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\Hash.c
# End Source File
# Begin Source File

SOURCE=..\Holder.c
# End Source File
# Begin Source File

SOURCE=..\HoldInstructionCode.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\HowCertAttribute.c
# End Source File
# Begin Source File

SOURCE=..\IetfAttrSyntax.c
# End Source File
# Begin Source File

SOURCE=..\InhibitAnyPolicy.c
# End Source File
# Begin Source File

SOURCE=..\InvalidityDate.c
# End Source File
# Begin Source File

SOURCE=..\IssuerAltName.c
# End Source File
# Begin Source File

SOURCE=..\IssuerAndSerialNumber.c
# End Source File
# Begin Source File

SOURCE=..\IssuerSerial.c
# End Source File
# Begin Source File

SOURCE=..\IssuingDistributionPoint.c
# End Source File
# Begin Source File

SOURCE=..\KEKIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\KEKRecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\KeyAgreeRecipientIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\KeyAgreeRecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\KeyDerivationAlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\KeyEncryptionAlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\KeyIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\KeyPurposeId.c
# End Source File
# Begin Source File

SOURCE=..\KeyTransRecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\KeyUsage.c
# End Source File
# Begin Source File

SOURCE=..\LocalPostalAttributes.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\MandatedUnsignedAttr.c
# End Source File
# Begin Source File

SOURCE=..\MessageAuthenticationCode.c
# End Source File
# Begin Source File

SOURCE=..\MessageAuthenticationCodeAlgorithm.c
# End Source File
# Begin Source File

SOURCE=..\MessageDigest.c
# End Source File
# Begin Source File

SOURCE=..\MessageImprint.c
# End Source File
# Begin Source File

SOURCE=..\Name.c
# End Source File
# Begin Source File

SOURCE=..\NameConstraints.c
# End Source File
# Begin Source File

SOURCE=..\NetworkAddress.c
# End Source File
# Begin Source File

SOURCE=..\NoticeReference.c
# End Source File
# Begin Source File

SOURCE=..\NumericUserIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\ObjectDigestInfo.c
# End Source File
# Begin Source File

SOURCE=..\ORAddress.c
# End Source File
# Begin Source File

SOURCE=..\OrganizationalUnitName.c
# End Source File
# Begin Source File

SOURCE=..\OrganizationalUnitNames.c
# End Source File
# Begin Source File

SOURCE=..\OrganizationName.c
# End Source File
# Begin Source File

SOURCE=..\OriginatorIdentifierOrKey.c
# End Source File
# Begin Source File

SOURCE=..\OriginatorInfo.c
# End Source File
# Begin Source File

SOURCE=..\OriginatorPublicKey.c
# End Source File
# Begin Source File

SOURCE=..\OtherCertificateFormat.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\OtherHashAlgAndValue.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\OtherHashValue.c
# End Source File
# Begin Source File

SOURCE=..\OtherKeyAttribute.c
# End Source File
# Begin Source File

SOURCE=..\OtherRecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\OtherRevocationInfoFormat.c
# End Source File
# Begin Source File

SOURCE=..\PasswordRecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\PathLenConstraint.c
# End Source File
# Begin Source File

SOURCE=..\PDSName.c
# End Source File
# Begin Source File

SOURCE=..\PDSParameter.c
# End Source File
# Begin Source File

SOURCE=..\PersonalName.c
# End Source File
# Begin Source File

SOURCE=..\PhysicalDeliveryCountryName.c
# End Source File
# Begin Source File

SOURCE=..\PhysicalDeliveryOfficeName.c
# End Source File
# Begin Source File

SOURCE=..\PhysicalDeliveryOfficeNumber.c
# End Source File
# Begin Source File

SOURCE=..\PhysicalDeliveryOrganizationName.c
# End Source File
# Begin Source File

SOURCE=..\PhysicalDeliveryPersonalName.c
# End Source File
# Begin Source File

SOURCE=..\PKIFailureInfo.c
# End Source File
# Begin Source File

SOURCE=..\PKIFreeText.c
# End Source File
# Begin Source File

SOURCE=..\PKIStatus.c
# End Source File
# Begin Source File

SOURCE=..\PKIStatusInfo.c
# End Source File
# Begin Source File

SOURCE=..\PolicyConstraints.c
# End Source File
# Begin Source File

SOURCE=..\PolicyInformation.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\PolicyIssuerName.c
# End Source File
# Begin Source File

SOURCE=..\PolicyMappings.c
# End Source File
# Begin Source File

SOURCE=..\PolicyQualifierId.c
# End Source File
# Begin Source File

SOURCE=..\PolicyQualifierInfo.c
# End Source File
# Begin Source File

SOURCE=..\PostalAddress.c
# End Source File
# Begin Source File

SOURCE=..\PostalCode.c
# End Source File
# Begin Source File

SOURCE=..\PosteRestanteAddress.c
# End Source File
# Begin Source File

SOURCE=..\PostOfficeBoxAddress.c
# End Source File
# Begin Source File

SOURCE=..\PresentationAddress.c
# End Source File
# Begin Source File

SOURCE=..\PrivateDomainName.c
# End Source File
# Begin Source File

SOURCE=..\PrivateKeyUsagePeriod.c
# End Source File
# Begin Source File

SOURCE=..\ProxyInfo.c
# End Source File
# Begin Source File

SOURCE=..\RDNSequence.c
# End Source File
# Begin Source File

SOURCE=..\ReasonFlags.c
# End Source File
# Begin Source File

SOURCE=..\RecipientEncryptedKey.c
# End Source File
# Begin Source File

SOURCE=..\RecipientEncryptedKeys.c
# End Source File
# Begin Source File

SOURCE=..\RecipientIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\RecipientInfo.c
# End Source File
# Begin Source File

SOURCE=..\RecipientInfos.c
# End Source File
# Begin Source File

SOURCE=..\RecipientKeyIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\RelativeDistinguishedName.c
# End Source File
# Begin Source File

SOURCE=..\RevocationInfoChoice.c
# End Source File
# Begin Source File

SOURCE=..\RevocationInfoChoices.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\RevReq.c
# End Source File
# Begin Source File

SOURCE=..\RoleSyntax.c
# End Source File
# Begin Source File

SOURCE=..\SecurityCategory.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SelectedCommitmentTypes.c
# End Source File
# Begin Source File

SOURCE=..\Signature.c
# End Source File
# Begin Source File

SOURCE=..\SignatureAlgorithmIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignaturePolicy.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignaturePolicyId.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignaturePolicyIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignaturePolicyImplied.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignatureValidationPolicy.c
# End Source File
# Begin Source File

SOURCE=..\SignatureValue.c
# End Source File
# Begin Source File

SOURCE=..\SignedAttributes.c
# End Source File
# Begin Source File

SOURCE=..\SignedData.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignerAndVerifierRules.c
# End Source File
# Begin Source File

SOURCE=..\SignerIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\SignerInfo.c
# End Source File
# Begin Source File

SOURCE=..\SignerInfos.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignerRules.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigningCertificate.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigningCertTrustCondition.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigningPeriod.c
# End Source File
# Begin Source File

SOURCE=..\SigningTime.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignPolExtensions.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignPolExtn.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignPolicyHash.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignPolicyId.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SignPolicyInfo.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigPolicyHash.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigPolicyId.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigPolicyQualifierId.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SigPolicyQualifierInfo.c
# End Source File
# Begin Source File

SOURCE=..\SkipCerts.c
# End Source File
# Begin Source File

SOURCE=..\Slot.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SPuri.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\SPUserNotice.c
# End Source File
# Begin Source File

SOURCE=..\StreetAddress.c
# End Source File
# Begin Source File

SOURCE=..\SubjectAltName.c
# End Source File
# Begin Source File

SOURCE=..\SubjectDirectoryAttributes.c
# End Source File
# Begin Source File

SOURCE=..\SubjectInfoAccessSyntax.c
# End Source File
# Begin Source File

SOURCE=..\SubjectKeyIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\SubjectPublicKeyInfo.c
# End Source File
# Begin Source File

SOURCE=..\SvceAuthInfo.c
# End Source File
# Begin Source File

SOURCE=..\Target.c
# End Source File
# Begin Source File

SOURCE=..\TargetCert.c
# End Source File
# Begin Source File

SOURCE=..\Targets.c
# End Source File
# Begin Source File

SOURCE=..\TBSCertificate.c
# End Source File
# Begin Source File

SOURCE=..\TBSCertList.c
# End Source File
# Begin Source File

SOURCE=..\TeletexCommonName.c
# End Source File
# Begin Source File

SOURCE=..\TeletexDomainDefinedAttribute.c
# End Source File
# Begin Source File

SOURCE=..\TeletexDomainDefinedAttributes.c
# End Source File
# Begin Source File

SOURCE=..\TeletexOrganizationalUnitName.c
# End Source File
# Begin Source File

SOURCE=..\TeletexOrganizationalUnitNames.c
# End Source File
# Begin Source File

SOURCE=..\TeletexOrganizationName.c
# End Source File
# Begin Source File

SOURCE=..\TeletexPersonalName.c
# End Source File
# Begin Source File

SOURCE=..\TerminalIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\TerminalType.c
# End Source File
# Begin Source File

SOURCE=..\Time.c
# End Source File
# Begin Source File

SOURCE=..\TimeStampReq.c
# End Source File
# Begin Source File

SOURCE=..\TimeStampResp.c
# End Source File
# Begin Source File

SOURCE=..\TimeStampToken.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\TimestampTrustCondition.c
# End Source File
# Begin Source File

SOURCE=..\TSAPolicyId.c
# End Source File
# Begin Source File

SOURCE=..\TSTInfo.c
# End Source File
# Begin Source File

SOURCE=..\UnauthAttributes.c
# End Source File
# Begin Source File

SOURCE=..\UnformattedPostalAddress.c
# End Source File
# Begin Source File

SOURCE=..\UniqueIdentifier.c
# End Source File
# Begin Source File

SOURCE=..\UniquePostalName.c
# End Source File
# Begin Source File

SOURCE=..\UnizetoAttrSyntax.c
# End Source File
# Begin Source File

SOURCE=..\UnprotectedAttributes.c
# End Source File
# Begin Source File

SOURCE=..\UnsignedAttributes.c
# End Source File
# Begin Source File

SOURCE=..\UserKeyingMaterial.c
# End Source File
# Begin Source File

SOURCE=..\UserNotice.c
# End Source File
# Begin Source File

SOURCE=..\V2Form.c
# End Source File
# Begin Source File

SOURCE=..\Validity.c
# End Source File
# Begin Source File

SOURCE=..\SigPolicy\VerifierRules.c
# End Source File
# Begin Source File

SOURCE=..\Version.c
# End Source File
# Begin Source File

SOURCE=..\X121Address.c
# End Source File
# Begin Source File

SOURCE=..\X520CommonName.c
# End Source File
# Begin Source File

SOURCE=..\X520countryName.c
# End Source File
# Begin Source File

SOURCE=..\X520dnQualifier.c
# End Source File
# Begin Source File

SOURCE=..\X520LocalityName.c
# End Source File
# Begin Source File

SOURCE=..\X520name.c
# End Source File
# Begin Source File

SOURCE=..\X520OrganizationalUnitName.c
# End Source File
# Begin Source File

SOURCE=..\X520OrganizationName.c
# End Source File
# Begin Source File

SOURCE=..\X520Pseudonym.c
# End Source File
# Begin Source File

SOURCE=..\X520SerialNumber.c
# End Source File
# Begin Source File

SOURCE=..\X520StateOrProvinceName.c
# End Source File
# Begin Source File

SOURCE=..\X520Title.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AAControls.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AcceptablePolicySet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AccessDescription.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ACClearAttrs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Accuracy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AdministrationDomainName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AlgAndLength.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AlgorithmConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AlgorithmConstraintSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AnotherName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttCertIssuer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttCertValidityPeriod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttCertVersion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Attribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributeCertificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributeCertificateInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AttributeConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributesSequence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AttributeTrustCondition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributeType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributeTypeAndValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AttributeTypeConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttributeValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\AttributeValueConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AttrSpec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AuthAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AuthenticatedData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AuthorityInfoAccessSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\AuthorityKeyIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BaseCRLNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BaseDistance.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BasicConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BMDGroup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\bmdgroup_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BMDGroupPair.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BMDGroupPairList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BMDGroups.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BMDkeyIV.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BuiltInDomainDefinedAttribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BuiltInDomainDefinedAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\BuiltInStandardAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Certificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificateChoices.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificateIssuer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificateList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificatePolicies.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificateSerialNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertificateSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CertificateTrustPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CertificateTrustTrees.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CertInfoReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CertPolicyId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CertRefReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CertRevReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ClassList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Clearance.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CMSAttrs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CMSVersion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CommitmentRule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CommitmentRules.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CommitmentType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CommitmentTypeIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CommonName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\CommonRules.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CommonUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ContentEncryptionAlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ContentInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ContentType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Countersignature.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CountryName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CPSuri.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CRLDistributionPoints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CRLNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\CRLReason.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\DeltaTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Digest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DigestAlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DigestAlgorithmIdentifiers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DigestedData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DigestInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DirectoryString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DisplayText.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DistinguishedName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DistributionPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DistributionPointName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\DomainComponent.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdasn1_common\Dss-Parms.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EDIPartyName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EmailAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EncapsulatedContentInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EncryptedContent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EncryptedContentInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EncryptedData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EncryptedKey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\EnuRevReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\EnvelopedData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\ESSCertID.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtendedCertificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtendedCertificateInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtendedCertificateOrCertificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtendedNetworkAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Extension.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtensionAttribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtensionAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtensionORAddressComponents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtensionPhysicalDeliveryAddressComponents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Extensions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ExtKeyUsageSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\FieldOfApplication.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\FreshestCRL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\GeneralName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\GeneralNames.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\GeneralSubtree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\GeneralSubtrees.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\Hash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Holder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\HoldInstructionCode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\HowCertAttribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\IetfAttrSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\InhibitAnyPolicy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\InvalidityDate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\IssuerAltName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\IssuerAndSerialNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\IssuerSerial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\IssuingDistributionPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KEKIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KEKRecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyAgreeRecipientIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyAgreeRecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyDerivationAlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyEncryptionAlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyPurposeId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyTransRecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\KeyUsage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\LocalPostalAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\MandatedUnsignedAttr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\MessageAuthenticationCode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\MessageAuthenticationCodeAlgorithm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\MessageDigest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\MessageImprint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Name.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\NameConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\NetworkAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\NoticeReference.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\NumericUserIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ObjectDigestInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ORAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OrganizationalUnitName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OrganizationalUnitNames.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OrganizationName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OriginatorIdentifierOrKey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OriginatorInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OriginatorPublicKey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OtherCertificateFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\OtherHashAlgAndValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\OtherHashValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OtherKeyAttribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OtherRecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\OtherRevocationInfoFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PasswordRecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\PathLenConstraint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PDSName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PDSParameter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PersonalName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PhysicalDeliveryCountryName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PhysicalDeliveryOfficeName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PhysicalDeliveryOfficeNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PhysicalDeliveryOrganizationName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PhysicalDeliveryPersonalName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PKIFailureInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PKIFreeText.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PKIStatus.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PKIStatusInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PolicyConstraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PolicyInformation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\PolicyIssuerName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PolicyMappings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PolicyQualifierId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PolicyQualifierInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PostalAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PostalCode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PosteRestanteAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PostOfficeBoxAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PresentationAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PrivateDomainName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\PrivateKeyUsagePeriod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ProxyInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RDNSequence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\ReasonFlags.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientEncryptedKey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientEncryptedKeys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientInfos.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RecipientKeyIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RelativeDistinguishedName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RevocationInfoChoice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RevocationInfoChoices.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\RevReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\RoleSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SecurityCategory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SelectedCommitmentTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Signature.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignatureAlgorithmIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignaturePolicy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignaturePolicyId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignaturePolicyIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignaturePolicyImplied.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignatureValidationPolicy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignatureValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignedAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignedData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignerAndVerifierRules.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignerIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignerInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SignerInfos.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignerRules.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigningCertificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigningCertTrustCondition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigningPeriod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigningTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignPolExtensions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignPolExtn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignPolicyHash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignPolicyId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SignPolicyInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigPolicyHash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigPolicyId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigPolicyQualifierId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SigPolicyQualifierInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SkipCerts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Slot.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SPuri.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\SPUserNotice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\StreetAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SubjectAltName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SubjectDirectoryAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SubjectInfoAccessSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SubjectKeyIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SubjectPublicKeyInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SvceAuthInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Target.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TargetCert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Targets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TBSCertificate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TBSCertList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexCommonName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexDomainDefinedAttribute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexDomainDefinedAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexOrganizationalUnitName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexOrganizationalUnitNames.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexOrganizationName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TeletexPersonalName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TerminalIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TerminalType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Time.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TimeStampReq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TimeStampResp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TimeStampToken.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\TimestampTrustCondition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TSAPolicyId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\TSTInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UnauthAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UnformattedPostalAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UniqueIdentifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UniquePostalName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UnizetoAttrSyntax.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UnprotectedAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UnsignedAttributes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UserKeyingMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\UserNotice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\V2Form.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Validity.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\SigPolicy\VerifierRules.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\Version.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X121Address.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520CommonName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520countryName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520dnQualifier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520LocalityName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520name.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520OrganizationalUnitName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520OrganizationName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520Pseudonym.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520SerialNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520StateOrProvinceName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_common\X520Title.h
# End Source File
# End Group
# End Target
# End Project
