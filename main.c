/* ========================================
 *
 *              ELECH410 labs
 *          FreeRTOS pandemic project
 *
 *                   2025
 *
 * ========================================
 */
#include "project.h"

/* RTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "pandemic.h"

#define TASK_STACK_SIZE (1024)

/* Task definitions */
#define GAME_TASK_NAME ("game_task")
#define QUARANTINE_TASK_NAME ("quarantine_task")
#define VACCINE_TASK_NAME ("vaccine_task")
#define MEDICINE_TASK_NAME ("medicine_production_task")
#define LCD_TASK_NAME ("lcd_task")

/* Priorities */
#define GAME_PRIORITY (20)
#define QUARANTINE_PRIORITY (19)
#define LCD_PRIORITY (18)
#define VACCINE_PRIORITY (17)
#define MEDICINE_PRIORITY (16)

#define MUTEX_TIMEOUT (475)

/* Task handlers */
TaskHandle_t gameHandler;
TaskHandle_t quarantineHandler;
TaskHandle_t medicineHandler;
TaskHandle_t vaccineHandler;
TaskHandle_t lcdHandler;

/* Task prototypes */
void quarantineTask(void *);
void medicineProductionTask(void *);
void vaccineProductionTask(void *);
void lcdTask(void *);
void deleteTasks(void);

/* Semaphores and mutexes */
SemaphoreHandle_t quarantineStart;
SemaphoreHandle_t vaccineStart;
SemaphoreHandle_t lab;
SemaphoreHandle_t lcd;

/* Global variables */
uint8_t lab_occupation_counter;
Token clueMain;
Token medicine;

/*
 * Installs the RTOS interrupt handlers.
 */
static void freeRTOSInit(void);

int main(void) {
  /* Enable global interrupts. */
  CyGlobalIntEnable;

  freeRTOSInit();

  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  LCD_Start();
  KB_Start();

  quarantineStart = xSemaphoreCreateBinary();
  vaccineStart = xSemaphoreCreateBinary();
  lab = xSemaphoreCreateMutex();
  lcd = xSemaphoreCreateBinary();

  // Create tasks
  xTaskCreate(gameTask, GAME_TASK_NAME, TASK_STACK_SIZE, NULL, GAME_PRIORITY,
              &gameHandler);
  xTaskCreate(quarantineTask, QUARANTINE_TASK_NAME, TASK_STACK_SIZE, NULL,
              QUARANTINE_PRIORITY, &quarantineHandler);
  xTaskCreate(vaccineProductionTask, VACCINE_TASK_NAME, TASK_STACK_SIZE, NULL,
              VACCINE_PRIORITY, &vaccineHandler);
  xTaskCreate(medicineProductionTask, MEDICINE_TASK_NAME, TASK_STACK_SIZE, NULL,
              MEDICINE_PRIORITY, &medicineHandler);
  xTaskCreate(lcdTask, LCD_TASK_NAME, TASK_STACK_SIZE, NULL, LCD_PRIORITY,
              &lcdHandler);

  // Log
  GPIOJ21_Write(1u);
  CyDelay(1u);
  GPIOJ21_Write(0u);

  // Launch freeRTOS
  vTaskStartScheduler();

  for (;;) {
  }
}

void freeRTOSInit(void) {
  /* Port layer functions that need to be copied into the vector table. */
  extern void xPortPendSVHandler(void);
  extern void xPortSysTickHandler(void);
  extern void vPortSVCHandler(void);
  extern cyisraddress CyRamVectors[];

  /* Install the OS Interrupt Handlers. */
  CyRamVectors[11] = (cyisraddress)vPortSVCHandler;
  CyRamVectors[14] = (cyisraddress)xPortPendSVHandler;
  CyRamVectors[15] = (cyisraddress)xPortSysTickHandler;
}

/*
 * When a contamination occurs gameTask calls this function.
 *
 */
void releaseContamination(void) {
  GPIOJ21_Write(1u);
  xSemaphoreGive(quarantineStart);
}

void quarantineTask(void *arg) {
  (void)arg;

  for (;;) {
    xSemaphoreTake(quarantineStart, portMAX_DELAY);
    quarantine();
    GPIOJ21_Write(0u);
  }
}

void medicineProductionTask(void *arg) {
  (void)arg;
  for (;;) {

    if (getVaccineCntr() >= 100) {
      vTaskDelete(medicineHandler);
    }

    if (xSemaphoreTake(lab, MUTEX_TIMEOUT) == pdTRUE) {
      GPIOJ11_Write(1u);
      medicine = assignMissionToLab(0);
      GPIOJ11_Write(0u);
      xSemaphoreGive(lab);
    }

    if (xSemaphoreTake(lab, MUTEX_TIMEOUT) == pdTRUE) {
      GPIOJ12_Write(1u);
      shipMedicine(medicine);
      GPIOJ12_Write(0u);
      xSemaphoreGive(lcd);
      xSemaphoreGive(lab);
    }
  }
}

/*
 * When gameTask releases a vaccine clue it calls this function.
 *
 */
void releaseClue(Token clue) {
  clueMain = clue;
  GPIOJ22_Write(1u);
  CyDelay(1u);
  GPIOJ22_Write(0u);
  xSemaphoreGive(vaccineStart);
}

void vaccineProductionTask(void *arg) {
  (void)arg;

  for (;;) {
    xSemaphoreTake(vaccineStart, portMAX_DELAY);
    if (xSemaphoreTake(lab, MUTEX_TIMEOUT)) {
      GPIOJ13_Write(1u);
      Token vaccine = assignMissionToLab(clueMain);
      shipVaccine(vaccine);
      GPIOJ13_Write(0u);
      xSemaphoreGive(lcd);
      xSemaphoreGive(lab);
    }
    if (getVaccineCntr() >= 100 || getPopulationCntr() == 0) {
      deleteTasks();
    }
  }
}

void deleteTasks(void) {
  vTaskDelete(quarantineHandler);
  vTaskDelete(medicineHandler);
  vTaskDelete(lcdHandler);
  vTaskDelete(vaccineHandler);
}

void lcdTask(void *arg) {
  (void)arg;
  for (;;) {
    xSemaphoreTake(lcd, portMAX_DELAY);
    GPIOJ14_Write(1u);
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintNumber(getPopulationCntr());
    LCD_PrintString(",");
    LCD_PrintNumber(getVaccineCntr());
    LCD_PrintString(",");
    LCD_PrintNumber(getMedicineCntr());
    GPIOJ14_Write(0u);
  }
}

/* [] END OF FILE */
