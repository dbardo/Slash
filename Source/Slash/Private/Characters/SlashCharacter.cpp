// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"

// Constructor for ASlashCharacter
ASlashCharacter::ASlashCharacter()
{
    // Enable ticking for the actor
    PrimaryActorTick.bCanEverTick = true;
    
    // Create and attach components
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    MovementComponent->bOrientRotationToMovement = true;
    MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);
    
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    Wig = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    Wig->SetupAttachment(GetMesh());
    Wig->AttachmentName = FString("head");

    Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = FString("head");

    // Setup default values for camera control
    bUseControllerRotationRoll = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;
}

void ASlashCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Axis mappings
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASlashCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASlashCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASlashCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &ASlashCharacter::Lookup);

    // Action mappings
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
    PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Move the character forward based on input value
void ASlashCharacter::MoveForward(float Value)
{
    // Check if attacking, and prevent movement during attack
    if (ActionState != EActionState::EAS_Unoccupied) { return; }
    
    if (Controller && Value != 0.f)
    {
        const FRotator ControlRotation = GetControlRotation();
        const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // Add forward movement based on input
        AddMovementInput(Direction, Value);
    }
}

// Move the character right based on input value
void ASlashCharacter::MoveRight(float Value)
{
    // Check if attacking, and prevent movement during attack
    if (ActionState != EActionState::EAS_Unoccupied) { return; }
    
    if (Controller && Value != 0.f)
    {
        const FRotator ControlRotation = GetControlRotation();
        const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // Add rightward movement based on input
        AddMovementInput(Direction, Value);
    }
}

// Turn the character based on input value
void ASlashCharacter::Turn(float Value)
{
    // Add yaw input for character rotation
    AddControllerYawInput(Value);
}

// Look up or down based on input value
void ASlashCharacter::Lookup(float Value)
{
    // Add pitch input for character rotation
    AddControllerPitchInput(Value);
}

// Handle the 'E' key press
void ASlashCharacter::EKeyPressed()
{
    // Attempt to cast the overlapping item to a weapon
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (OverlappingWeapon)
    {
        // Equip the weapon to the specified socket and update character state
        OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        OverlappingItem = nullptr;
        EquippedWeapon = OverlappingWeapon;
    }
    else
    {
        // Check if character can disarm or arm
        if (CanDisarm())
        {
            // Play unequip montage and update character state
            PlayEquipMontage(FName("Unequip"));
            CharacterState = ECharacterState::ECS_Unequipped;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if (CanArm())
        {
            // Play equip montage and update character state
            PlayEquipMontage(FName("Equip"));
            CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
}

// Check if the character can be disarmed
bool ASlashCharacter::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;
}

// Check if the character can arm
bool ASlashCharacter::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState == ECharacterState::ECS_Unequipped &&
        EquippedWeapon;
}

// Disarm the character
void ASlashCharacter::Disarm()
{
    // Attach the equipped weapon to the spine socket
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
    }
}

// Arm the character
void ASlashCharacter::Arm()
{
    // Attach the equipped weapon to the right hand socket
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), "RightHandSocket");
    }
}

void ASlashCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

// Initiate the attack
void ASlashCharacter::Attack()
{
    // Check if the character can attack
    if (CanAttack())
    {
        // Set action state to attacking and play the attack montage
        ActionState = EActionState::EAS_Attacking;
        PlayAttackMontage();
    }
}

// Check if the character can attack
bool ASlashCharacter::CanAttack()
{
    return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

// Play the attack montage
void ASlashCharacter::PlayAttackMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    
    if (AnimInstance && AttackMontage)
    {
        // Play the attack montage and jump to a random attack section
        AnimInstance->Montage_Play(AttackMontage);
        const int32 Selection = FMath::RandRange(0, 2);
        FName SectionName;
        switch (Selection)
        {
        case 0:
            SectionName = FName("Attack1");
            break;
        case 1:
            SectionName = FName("Attack2");
            break;
        case 2:
            SectionName = FName("Attack3");
            break;
        default:
            SectionName = FName("Attack1");
            break;
        }

        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
    }
}

// Play the equip montage with a specific section
void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && EquipMontage)
    {
        // Play the equip montage and jump to the specified section
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

// Called when the attack animation ends
void ASlashCharacter::AttackEnd()
{
    // Set the action state to unoccupied
    ActionState = EActionState::EAS_Unoccupied;
}

