AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdasn1_common.la

libbmdasn1_common_la_SOURCES = \
BMDGroupPair.c 			BMDGroup.c 		BMDGroups.c		BMDGroupPairList.c \
bmdgroup_utils.c                BMDLogElements.c \
AAControls.c                            ExtensionPhysicalDeliveryAddressComponents.c  ReasonFlags.c	\
AccessDescription.c                     Extensions.c                                  RecipientEncryptedKey.c	\
ACClearAttrs.c                          ExtKeyUsageSyntax.c                           RecipientEncryptedKeys.c	\
Accuracy.c                              FreshestCRL.c                                 RecipientIdentifier.c	\
AdministrationDomainName.c              GeneralName.c                                 RecipientInfo.c	\
AlgorithmIdentifier.c                   GeneralNames.c                                RecipientInfos.c	\
AnotherName.c                           GeneralSubtree.c                              RecipientKeyIdentifier.c	\
asn1_modules                            GeneralSubtrees.c                             RelativeDistinguishedName.c	\
AttCertIssuer.c                         Holder.c                                      RevocationInfoChoice.c	\
AttCertValidityPeriod.c                 HoldInstructionCode.c                         RevocationInfoChoices.c	\
AttCertVersion.c                        IetfAttrSyntax.c                              RoleSyntax.c	\
Attribute.c                             InhibitAnyPolicy.c                            SecurityCategory.c	\
AttributeCertificate.c                  InvalidityDate.c                              SignatureAlgorithmIdentifier.c	\
AttributeCertificateInfo.c              IssuerAltName.c                               Signature.c	\
AttributeTypeAndValue.c                 IssuerAndSerialNumber.c                       SignatureValue.c	\
AttributeType.c                         IssuerSerial.c                                SignedAttributes.c	\
AttributeValue.c                        IssuingDistributionPoint.c                    SignedData.c	\
AttrSpec.c                              KEKIdentifier.c                               SignerIdentifier.c	\
AuthAttributes.c                        KEKRecipientInfo.c                            SignerInfo.c	\
AuthenticatedData.c                     KeyAgreeRecipientIdentifier.c                 SignerInfos.c	\
AuthorityInfoAccessSyntax.c             KeyAgreeRecipientInfo.c                       SigningTime.c	\
AuthorityKeyIdentifier.c                KeyDerivationAlgorithmIdentifier.c            SkipCerts.c	\
BaseCRLNumber.c                         KeyEncryptionAlgorithmIdentifier.c            Slot.c	\
BaseDistance.c                          KeyIdentifier.c                               StreetAddress.c	\
BasicConstraints.c                      KeyPurposeId.c                                SubjectAltName.c	\
BuiltInDomainDefinedAttribute.c         KeyTransRecipientInfo.c                       SubjectDirectoryAttributes.c	\
BuiltInDomainDefinedAttributes.c        KeyUsage.c                                    SubjectInfoAccessSyntax.c	\
BuiltInStandardAttributes.c             LocalPostalAttributes.c                       SubjectKeyIdentifier.c	\
Certificate.c                           Makefile.am                                   SubjectPublicKeyInfo.c	\
CertificateChoices.c                    MessageAuthenticationCodeAlgorithm.c          SvceAuthInfo.c	\
CertificateIssuer.c                     MessageAuthenticationCode.c                   Target.c	\
CertificateList.c                       MessageDigest.c                               TargetCert.c	\
CertificatePolicies.c                   MessageImprint.c                              Targets.c	\
CertificateSerialNumber.c               Name.c                                        TBSCertificate.c	\
CertificateSet.c                        NameConstraints.c                             TBSCertList.c	\
CertPolicyId.c                          NetworkAddress.c                              TeletexCommonName.c	\
ClassList.c                             NoticeReference.c                             TeletexDomainDefinedAttribute.c	\
Clearance.c                             NumericUserIdentifier.c                       TeletexDomainDefinedAttributes.c	\
CMSVersion.c                            ObjectDigestInfo.c                            TeletexOrganizationalUnitName.c	\
CommonName.c                            ORAddress.c                                   TeletexOrganizationalUnitNames.c	\
ContentEncryptionAlgorithmIdentifier.c  OrganizationalUnitName.c                      TeletexOrganizationName.c	\
ContentInfo.c                           OrganizationalUnitNames.c                     TeletexPersonalName.c	\
ContentType.c                           OrganizationName.c                            TerminalIdentifier.c	\
Countersignature.c                      OriginatorIdentifierOrKey.c                   TerminalType.c	\
CountryName.c                           OriginatorInfo.c                              Time.c	\
CPSuri.c                                OriginatorPublicKey.c                         TimeStampReq.c	\
CRLDistributionPoints.c                 OtherCertificateFormat.c                      TimeStampResp.c	\
CRLNumber.c                             OtherKeyAttribute.c                           TimeStampToken.c	\
CRLReason.c                             OtherRecipientInfo.c                          TSAPolicyId.c	\
DigestAlgorithmIdentifier.c             OtherRevocationInfoFormat.c                   TSTInfo.c	\
DigestAlgorithmIdentifiers.c            PasswordRecipientInfo.c                       UnauthAttributes.c	\
Digest.c                                PDSName.c                                     UnformattedPostalAddress.c	\
DigestedData.c                          PDSParameter.c                                UniqueIdentifier.c	\
DirectoryString.c                       PersonalName.c                                UniquePostalName.c	\
DisplayText.c                           PhysicalDeliveryCountryName.c                 UnizetoAttrSyntax.c	\
DistinguishedName.c                     PhysicalDeliveryOfficeName.c                  UnprotectedAttributes.c	\
DistributionPoint.c                     PhysicalDeliveryOfficeNumber.c                UnsignedAttributes.c	\
DistributionPointName.c                 PhysicalDeliveryOrganizationName.c            UserKeyingMaterial.c	\
DomainComponent.c                       PhysicalDeliveryPersonalName.c                UserNotice.c	\
Dss-Parms.c                             PKIFailureInfo.c                              V2Form.c	\
EDIPartyName.c                          PKIFreeText.c                                 Validity.c	\
EmailAddress.c                          PKIStatus.c                                   Version.c	\
EncapsulatedContentInfo.c               PKIStatusInfo.c                               X121Address.c	\
EncryptedContent.c                      PolicyConstraints.c                           X520CommonName.c	\
EncryptedContentInfo.c                  PolicyInformation.c                           X520countryName.c	\
EncryptedData.c                         PolicyMappings.c                              X520dnQualifier.c	\
EncryptedKey.c                          PolicyQualifierId.c                           X520LocalityName.c	\
EnvelopedData.c                         PolicyQualifierInfo.c                         X520name.c	\
ExtendedCertificate.c                   PostalCode.c                                  X520OrganizationalUnitName.c	\
ExtendedCertificateInfo.c               PosteRestanteAddress.c                        X520OrganizationName.c	\
ExtendedCertificateOrCertificate.c      PostOfficeBoxAddress.c                        X520Pseudonym.c	\
ExtendedNetworkAddress.c                PresentationAddress.c                         X520SerialNumber.c	\
ExtensionAttribute.c                    PrivateDomainName.c                           X520StateOrProvinceName.c	\
ExtensionAttributes.c                   PrivateKeyUsagePeriod.c                       X520Title.c	\
Extension.c                             ProxyInfo.c                                   CommonUtils.c	\
ExtensionORAddressComponents.c          RDNSequence.c				      PostalAddress.c DigestInfo.c \
BMDkeyIV.c AttributesSequence.c \
\
SigPolicy/OtherHashValue.c	SigPolicy/OtherHashAlgAndValue.c	SigPolicy/SPUserNotice.c \
SigPolicy/SPuri.c		SigPolicy/SigPolicyQualifierId.c	SigPolicy/SigPolicyQualifierInfo.c \
SigPolicy/SigPolicyHash.c	SigPolicy/SigPolicyId.c			SigPolicy/SignaturePolicyImplied.c \
SigPolicy/SignaturePolicyId.c	SigPolicy/SignaturePolicyIdentifier.c \
\
SigPolicy/AcceptablePolicySet.c		SigPolicy/AlgAndLength.c		SigPolicy/AlgorithmConstraintSet.c \
SigPolicy/AlgorithmConstraints.c	SigPolicy/AttributeConstraints.c	SigPolicy/AttributeTrustCondition.c \
SigPolicy/AttributeTypeConstraints.c	SigPolicy/AttributeValueConstraints.c	SigPolicy/CMSAttrs.c \
SigPolicy/CertInfoReq.c			SigPolicy/CertRefReq.c			SigPolicy/CertRevReq.c \
SigPolicy/CertificateTrustPoint.c	SigPolicy/CertificateTrustTrees.c	SigPolicy/CommitmentRule.c \
SigPolicy/CommitmentRules.c		SigPolicy/CommitmentType.c		SigPolicy/CommitmentTypeIdentifier.c \
SigPolicy/CommonRules.c			SigPolicy/DeltaTime.c			SigPolicy/EnuRevReq.c \
SigPolicy/FieldOfApplication.c		SigPolicy/HowCertAttribute.c		SigPolicy/MandatedUnsignedAttr.c \
SigPolicy/PathLenConstraint.c		SigPolicy/PolicyIssuerName.c		SigPolicy/RevReq.c \
SigPolicy/SelectedCommitmentTypes.c	SigPolicy/SignPolExtensions.c		SigPolicy/SignPolExtn.c \
SigPolicy/SignPolicyHash.c		SigPolicy/SignPolicyId.c		SigPolicy/SignPolicyInfo.c \
SigPolicy/SignaturePolicy.c		SigPolicy/SignatureValidationPolicy.c	SigPolicy/SignerAndVerifierRules.c \
SigPolicy/SignerRules.c			SigPolicy/SigningCertTrustCondition.c	SigPolicy/SigningPeriod.c \
SigPolicy/TimestampTrustCondition.c	SigPolicy/VerifierRules.c \
\
SigPolicy/ESSCertID.c			SigPolicy/Hash.c			SigPolicy/SigningCertificate.c \
\
AcceptableResponses.c  CertStatus.c              DVCSTime.c     OCSPResponse.c        ResponseBytes.c                 ServiceLocator.c \
ArchiveCutoff.c        DVCSCertInfo.c            Data.c         OCSPResponseStatus.c  ResponseData.c                  ServiceType.c \
BasicOCSPResponse.c    DVCSErrorNotice.c         IV.c           PathProcInput.c       RevokedInfo.c                   SingleResponse.c \
CBCParameter.c         DVCSRequest.c             KeyHash.c      RC2-CBC-parameter.c   SMIMECapabilities.c             TBSRequest.c \
CertEtcToken.c         DVCSRequestInformation.c  Nonce.c        Request.c             SMIMECapability.c               TargetEtcChain.c \
CertID.c               DVCSResponse.c            OCSPRequest.c  ResponderID.c         SMIMEEncryptionKeyPreference.c  UnknownInfo.c \
MessageInfo.c \
\
pkcs15/CertId.c                             pkcs15/PKCS15CommonObjectFlags.c           pkcs15/PKCS15Operations.c                pkcs15/PKCS15Reference.c \
pkcs15/OOBCertHash.c                        pkcs15/PKCS15CommonPrivateKeyAttributes.c  pkcs15/PKCS15PasswordInfo.c              pkcs15/PKCS15ReferencedValue.c \
pkcs15/PBKDF2-params.c                      pkcs15/PKCS15CredentialIdentifier.c        pkcs15/PKCS15Path.c                      pkcs15/PKCS15Salt.c \
pkcs15/PKCS15AccessControlRule.c            pkcs15/PKCS15DigestInfoWithDefault.c       pkcs15/PKCS15PrivateKeyType.c            pkcs15/PKCS15SecurityCondition.c \
pkcs15/PKCS15AccessMode.c                   pkcs15/PKCS15EncryptedContentInfo.c        pkcs15/PKCS15PrivateKeys.c               pkcs15/PKCS15Token.c \
pkcs15/PKCS15CertificateObject.c            pkcs15/PKCS15EnvelopedData.c               pkcs15/PKCS15PrivateRSAKeyAttributes.c   pkcs15/PKCS15URL.c \
pkcs15/PKCS15CertificateType.c              pkcs15/PKCS15Identifier.c                  pkcs15/PKCS15PrivateRSAKeyObject.c       pkcs15/PKCS15Usage.c \
pkcs15/PKCS15CertificateValue.c             pkcs15/PKCS15KeyAccessFlags.c              pkcs15/PKCS15PublicKeyOperations.c       pkcs15/PKCS15X509CertificateAttributes.c \
pkcs15/PKCS15Certificates.c                 pkcs15/PKCS15KeyManagementInfo.c           pkcs15/PKCS15RSAParamsAndOps.c           pkcs15/PKCS15keyInfo.c \
pkcs15/PKCS15CommonCertificateAttributes.c  pkcs15/PKCS15KeyUsageFlags.c               pkcs15/PKCS15RSAPrivateKeyInfo.c         pkcs15/PKCS15urlWithDigest.c \
pkcs15/PKCS15CommonKeyAttributes.c          pkcs15/PKCS15Label.c                       pkcs15/PKCS15RSAPrivateKeyObject.c       pkcs15/RSAPrivateKey.c \
pkcs15/PKCS15CommonObjectAttributes.c       pkcs15/PKCS15Objects.c                     pkcs15/PKCS15RSAPrivateKeyObjectValue.c  pkcs15/RSAPublicKey.c

