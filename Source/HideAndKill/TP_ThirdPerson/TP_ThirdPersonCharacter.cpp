// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "../BaseCharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Create kill area
	KillArea = CreateDefaultSubobject<UBoxComponent>(TEXT("KillArea"));
	KillArea->SetupAttachment(RootComponent);
}

void ATP_ThirdPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Enable Kill ability
	CanKill = true;

	// Kill area
	KillArea->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATP_ThirdPersonCharacter::OnKillTargetBeginOverlap);
	KillArea->OnComponentEndOverlap.AddUniqueDynamic(this, &ATP_ThirdPersonCharacter::OnKillTargetEndOverlap);
}

AActor* ATP_ThirdPersonCharacter::GetKillTarget() const
{
	if (KillTargetCandidates.IsEmpty()) return nullptr;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Get nearest target to player
	AActor* KillTarget = nullptr;
	for (auto Candidate : KillTargetCandidates)
	{
		// Raycast (to check there is no wall between player and target)
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), Candidate->GetActorLocation(), ECC_Pawn, QueryParams);
		if (HitResult.GetActor() != Candidate)
		{
			continue;
		}

		// Check if candidate is closer to player
		if (KillTarget == nullptr) {
			KillTarget = Candidate;
			continue;
		}
		if (this->GetHorizontalDistanceTo(Candidate) < this->GetHorizontalDistanceTo(KillTarget))
		{
			KillTarget = Candidate;
		}
	}
	return KillTarget;
}

void ATP_ThirdPersonCharacter::ServerReqKill_Implementation(AActor* Target)
{
	// Get killable target
	AActor* TargetToKill = GetKillTarget();
	if (TargetToKill)
	{
		if (TargetToKill->Implements<UKillable>())
		{
			// Verify if this target is the same as the client, to avoid unwanted assassination due to latency
			if (TargetToKill != Target) return;
			// Assassinate target
			MulticastKillTarget(TargetToKill);
		}
	}
}

void ATP_ThirdPersonCharacter::MulticastKillTarget_Implementation(AActor* Target)
{
	IKillable* TargetToKill = Cast<IKillable>(Target);
	if (TargetToKill)
	{
		TargetToKill->OnKill(this);
	}
}

bool ATP_ThirdPersonCharacter::ServerReqKill_Validate(AActor* Target)
{
	return true;
}

void ATP_ThirdPersonCharacter::OnKillTargetBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IKillable>(OtherActor) && OtherActor != this) // TODO filter with Object Channel instead ? 
	{
		KillTargetCandidates.Add(OtherActor);
	}
}

void ATP_ThirdPersonCharacter::OnKillTargetEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (KillTargetCandidates.Contains(OtherActor))
	{
		KillTargetCandidates.Remove(OtherActor);
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Move);
		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABaseCharacter::SetSprinting, true);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABaseCharacter::SetSprinting, false);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Look);

		//Kill
		EnhancedInputComponent->BindAction(KillAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Kill);

	}

}

void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	bool Moving = false;

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (!IsFreelooking)
		{

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
		}
		else
		{
			AddMovementInput(GetActorForwardVector(), MovementVector.Y);

			if (!GetVelocity().IsZero() && CanTurn)
			{

				AddMovementInput(GetActorRightVector()/20, MovementVector.X);
			}
			else {
				// get right vector 
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
		}
	}
}

void ATP_ThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATP_ThirdPersonCharacter::FreeLook(const FInputActionValue& Value)
{
	//Move freelook to c++ here if needed 
}

void ATP_ThirdPersonCharacter::Kill(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Want to kill !"));
	AActor* TargetToKill = GetKillTarget();
	if (TargetToKill && CanKill)
	{
		ServerReqKill(TargetToKill);
		GetWorld()->GetTimerManager().SetTimer(KillHandle, this, &ATP_ThirdPersonCharacter::RefreshCanKill, KillCooldown, false);
		CanKill = false;
	}
}

void ATP_ThirdPersonCharacter::RefreshCanKill(){
	CanKill = true;
}




