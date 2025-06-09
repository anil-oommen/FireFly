import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { WebcontrolComponent } from './webcontrol.component';

describe('WebcontrolComponent', () => {
  let component: WebcontrolComponent;
  let fixture: ComponentFixture<WebcontrolComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ WebcontrolComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(WebcontrolComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
